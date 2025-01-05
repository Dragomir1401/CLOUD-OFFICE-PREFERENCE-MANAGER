import json
from flask import Flask, render_template, request, jsonify
from collections import Counter
import matplotlib.pyplot as plt
from datetime import datetime
import io
import os
import base64
import requests
from base64 import b64decode, b64encode
from Cryptodome.Cipher import AES

# AES key and IV
aes_key = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10'
iv = b'\x00' * 16

def pad(data):
    """Apply PKCS#7 padding."""
    padding_len = 16 - (len(data) % 16)
    return data + (chr(padding_len) * padding_len).encode('utf-8')

def unpad(data):
    """Remove PKCS#7 padding."""
    padding_len = data[-1]
    return data[:-padding_len]

def encrypt_aes(plain_text):
    """Encrypt plaintext using AES (CBC mode) and Base64-encode."""
    # Convert plaintext to bytes and pad
    plain_bytes = plain_text.encode('utf-8')
    padded_text = pad(plain_bytes)
    
    # Create AES cipher
    cipher = AES.new(aes_key, AES.MODE_CBC, iv)
    
    # Encrypt and encode to Base64
    encrypted_bytes = cipher.encrypt(padded_text)
    return b64encode(encrypted_bytes).decode('utf-8')

def decrypt_aes(encrypted_text):
    """Decrypt Base64-encoded ciphertext using AES (CBC mode)."""
    # Decode Base64 input
    encrypted_bytes = b64decode(encrypted_text)
    
    # Create AES cipher
    cipher = AES.new(aes_key, AES.MODE_CBC, iv)
    
    # Decrypt and remove padding
    decrypted_bytes = cipher.decrypt(encrypted_bytes)
    return unpad(decrypted_bytes).decode('utf-8')

esp_ip_address = '192.168.0.180'

app = Flask(__name__)

# Load users from the JSON file
def load_users():
    with open('users.json', 'r') as f:
        data = json.load(f)
    return data['users']

# Save users to the JSON file
def save_users():
    with open('users.json', 'w') as f:
        json.dump({"users": users}, f, indent=4)

# Store users in a global variable (for simplicity)
users = load_users()

# Load mock logs at the start of the server
def load_mock_logs():
    mock_logs_path = os.path.join(os.path.dirname(__file__), 'mock_logs.json')
    if os.path.exists(mock_logs_path):
        with open(mock_logs_path, 'r') as f:
            return json.load(f)
    return []

# Temporary list to store employee logs
logs = load_mock_logs()

@app.route('/')
def menu():
    return render_template('menu.html')

@app.route('/users')
def users_page():
    with open('users.json') as f:
        users = json.load(f)['users']
    return render_template('users.html', users=users)

@app.route('/preferences')
def preferences_page():
    return render_template('preferences.html', users=users)

@app.route('/records')
def records_page():
    # Pass the logs to the records page
    return render_template('records.html', logs=logs)

@app.route('/visualization')
def visualization_page():
    from collections import Counter

    # Parse and sort logs by time
    sorted_logs = sorted(logs, key=lambda log: datetime.strptime(log['time'], "%m/%d/%Y %H:%M:%S"))

    # Extract times, temperatures, and humidities
    times = [log['time'] for log in sorted_logs if 'time' in log]
    temperatures = [log['temperature'] for log in sorted_logs if 'temperature' in log]
    humidities = [log['humidity'] for log in sorted_logs if 'humidity' in log]

    # User Preferences
    user_names = [user['name'] for user in users]
    user_temperatures = [user['preferences']['temperature'] for user in users]
    user_humidities = [user['preferences']['humidity'] for user in users]

    # All Hours Graph
    crowded_times = [log['time'].split(':')[0] for log in logs if 'time' in log]
    crowded_times_count = Counter(crowded_times)

    crowded_times_labels = list(crowded_times_count.keys())
    crowded_times_data = list(crowded_times_count.values())

    # Top Visitors
    top_visitors_count = Counter([log['name'] for log in logs if 'name' in log])
    top_visitors_labels = list(top_visitors_count.keys())
    top_visitors_data = list(top_visitors_count.values())

    return render_template(
        'visualization.html',
        user_names=user_names,
        user_temperatures=user_temperatures,
        user_humidities=user_humidities,
        times=times,
        temperatures=temperatures,
        humidities=humidities,
        crowded_times_labels=crowded_times_labels,
        crowded_times_data=crowded_times_data,
        top_visitors_labels=top_visitors_labels,
        top_visitors_data=top_visitors_data
    )


@app.route('/login_employee/', methods=['POST'])
def log_employee():
    # Parse the JSON data sent by the ESP32
    data = request.get_json()
    data = decrypt_aes(data)
    data['type'] = 'login'

    # Store the incoming log in the logs list
    logs.append(data)

    return jsonify({"status": "success"})

@app.route('/logout_employee/', methods=['POST'])
def logout_employee():
    # Parse the JSON data sent by the ESP32
    data = request.get_json()
    data = decrypt_aes(data)
    data['type'] = 'logout'

    # Store the incoming log in the logs list
    logs.append(data)

    return jsonify({"status": "success"})

@app.route('/update_preferences', methods=['POST'])
def update_preferences():
    data = request.get_json()

    # Find the user by UID
    user_id = data.get('id')
    user = next((u for u in users if u['id'] == user_id), None)
    
    if user:
        # Update the preferences if the user exists
        user['preferences'] = data.get('preferences', user['preferences'])
        user['access'] = data.get('access', user['access'])
        user['reminder'] = data.get('reminder', user['reminder'])
        
        save_users()  # Save the changes to the file
        
        # send to esp an event signaling the update
        payload = {"type": "update",
                   "name": user['name'],
                   "access": user['access'],
                   "reminder": user['reminder'],
                   "preferences": user['preferences']}
        requests.post(f'http://{esp_ip_address}/event', json=encrypt_aes(json.dumps(payload)))
            
        
        return jsonify({"status": "success", "message": "User preferences updated!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404

# New route for reminders
@app.route('/set_reminder', methods=['POST'])
def set_reminder():
    data = request.get_json()
    data = decrypt_aes(data)

    # Find the user by UID
    user_id = data.get('id')
    user = next((u for u in users if u['id'] == user_id), None)

    if user:
        # Set the reminder if the user exists
        user['reminder'] = data.get('reminder', user['reminder'])
        
        save_users()  # Save the changes to the file
        
        return jsonify({"status": "success", "message": "Reminder set successfully!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/get_all_users_details', methods=['GET'])
def get_all_users_details():
    payload = json.dumps({"users": users})
    encrypted_payload = encrypt_aes(payload)
    return encrypted_payload, 200, {'Content-Type': 'text/plain'}

@app.route('/get_user_details/<int:user_id>', methods=['GET'])
def get_user_details(user_id):
    user = next((u for u in users if u['id'] == user_id), None)
    user = json.dumps(user)
    if user:
        return encrypt_aes(user)
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404

@app.route('/add_user', methods=['POST'])
def add_user():
    # Check if user already exists
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    # If user exists return 409 Conflict
    if any(u['id'] == user_id for u in users):
        return jsonify({"status": "failure", "message": "User already exists!"}), 409
    
    # Add the new user
    users.append(data)
    save_users()
    
    return jsonify({"status": "success", "message": "User added successfully!"})

@app.route('/remove_access', methods=['POST'])
def remove_access():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    # Check if user exists
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        user['access'] = False
        save_users()
        return jsonify({"status": "success", "message": "Access removed successfully!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/add_access', methods=['POST'])
def add_access():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    # Check if user exists
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        user['access'] = True
        save_users()
        return jsonify({"status": "success", "message": "Access granted successfully!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/check_access/', methods=['POST'])
def check_access():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        return jsonify({"access": user['access']})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/set_access/', methods=['POST'])
def set_access():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    access = data.get('access')
    
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        user['access'] = access
        save_users()
        return jsonify({"status": "success", "message": "Access updated successfully!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/get_user_name/', methods=['POST'])
def get_user_name():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        return jsonify({"name": user['name']})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/get_user_reminder/', methods=['POST'])
def get_reminder():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        return jsonify({"reminder": user['reminder']})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/get_user_preferences/', methods=['POST'])
def get_preferences():
    data = request.get_json()
    data = decrypt_aes(data)
    user_id = data.get('id')
    
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        return jsonify({"preferences": user['preferences']})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5001, debug=True, ssl_context=("cert.pem", "key.pem"))
