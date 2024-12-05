from flask import Flask, request, render_template

app = Flask(__name__)

# List to store employee login data
employee_data = []

# Route to handle incoming data from ESP
@app.route('/log_employee', methods=['POST'])
def log_employee():
    data = request.json
    print(data)
    if data:
        employee = {
            "name": data['name'],
            "uid": data['uid'],
            "time": data['time'],  # Time sent from ESP
            "temperature": data['temperature'],
            "humidity": data['humidity'],
            "reminder": data['reminder']
        }
        employee_data.append(employee)
        return {"status": "success"}, 200
    return {"status": "error", "message": "Invalid data"}, 400

# Route to display logged employees
@app.route('/')
def index():
    return render_template('index.html', employees=employee_data)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)

