import json
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Load users from the JSON file
def load_users():
    with open('users.json', 'r') as f:
        data = json.load(f)
    return data['users']

# Save users to the JSON file (to persist changes)
def save_users():
    with open('users.json', 'w') as f:
        json.dump({"users": users}, f, indent=4)

# Store users in a global variable (for simplicity)
users = load_users()

# Temporary list to store employee logs
logs = []

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

@app.route('/log_employee', methods=['POST'])
def log_employee():
    # Parse the JSON data sent by the ESP32
    data = request.get_json()

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
        
        return jsonify({"status": "success", "message": "User preferences updated!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404

# New route for reminders
@app.route('/set_reminder', methods=['POST'])
def set_reminder():
    data = request.get_json()

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
    
@app.route('/get_users_names_reminders', methods=['GET'])
def get_users_names_reminders():
    # Return a list of dictionaries with the user's name and reminder
    return jsonify([{"name": u['name'], "reminder": u['reminder']} for u in users])

@app.route('/add_user', methods=['POST'])
def add_user():
    # Check if user already exists
    data = request.get_json()
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
    user_id = data.get('id')
    
    # Check if user exists
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        user['access'] = True
        save_users()
        return jsonify({"status": "success", "message": "Access granted successfully!"})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    
@app.route('/check_access/<int:user_id>', methods=['GET'])
def check_access(user_id):
    user = next((u for u in users if u['id'] == user_id), None)
    if user:
        return jsonify({"access": user['access']})
    else:
        return jsonify({"status": "failure", "message": "User not found!"}), 404
    

if __name__ == '__main__':
    app.run(debug=True)
