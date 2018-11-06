from flask import Flask,render_template,flash,redirect,url_for,session,logging,request
from wtforms import Form,StringField,TextAreaField,PasswordField,validators
from passlib.hash import sha256_crypt
from functools import wraps
import paho.mqtt.publish as publish
import paho.mqtt.client as mqtt
import time,telepot,urllib3,socket

app=Flask(__name__)

#to establish the connectiopn import the MongoClient class
from flask_pymongo import PyMongo
app.config['MONGO_DBNAME']='signup'
app.config["MONGO_URI"] = "mongodb://localhost:27017/signup"
mongo = PyMongo(app)

###########to find ip address
import socket
my_ip=([l for l in ([ip for ip in socket.gethostbyname_ex(socket.gethostname())[2] 
if not ip.startswith("127.")][:1], [[(s.connect(('8.8.8.8', 53)), 
s.getsockname()[0], s.close()) for s in [socket.socket(socket.AF_INET, 
socket.SOCK_DGRAM)]][0][1]]) if l][0][0])


def on_connect(client, userdata,rc,self):
    print "Connected!", str(rc)
    client.subscribe("esp")
    client.subscribe("office/esp/gpio16")
    client.subscribe("office/esp/gpio5")
    client.subscribe("office/esp/gpio4")
    client.subscribe("office/esp/gpio2")
    client.subscribe("office/esp/gpio14")
    client.subscribe("office/esp/gpio12")
    client.subscribe("office/esp/gpio13")
    client.subscribe("office/esp/gpio15")
    print "after subscribe"

mqtt_broker_ip = my_ip
client = mqtt.Client()
app.secret_key = "super secret key"

#home
@app.route('/')
def index():
    return  render_template('home.html')

class RegisterForm(Form):
    name=StringField('Name',[validators.Length(min=1,max=50)])
    email=StringField('Email',[validators.Length(min=6,max=50)])
    password=PasswordField('Password',[
        validators.DataRequired(),
        validators.EqualTo('confirm',message='Passwords do not match')
    ])
    confirm=PasswordField('Confirm Password')

#user register
@app.route('/register',methods=['GET','POST'])
def register():
    #user register form
    form=RegisterForm(request.form)
    if request.method =='POST' and form.validate():
        name=form.name.data
        print(name)
        email=form.email.data
        password=sha256_crypt.encrypt(str(form.password.data))

        #create a colletion object and the collection name is users
        users=mongo.db.users
        existing_user_name=users.find_one({"name":form.name.data})
        if existing_user_name is None:
            users.insert({'name':form.name.data,'password':password,'email':form.email.data})
            session['username']=form.name.data
            flash('You are registered and can login','success')
            
            return redirect(url_for('login'))
    return render_template('register.html',form=form)


    #User Login
@app.route('/login',methods=['GET','POST'])
def login():
        if request.method=='POST':
            #Get form fields
            username=request.form['username']
            password_candidate=request.form['password']
            users=mongo.db.users
            login_user=users.find_one({"name":request.form['username']})

            if login_user is not None:
                print login_user
                #get stored hash
                password=login_user['password']

                #compare passwords
                if sha256_crypt.verify(password_candidate,password):
                    #passed
                    session['logged_in']=True
                    session['username']=username

                    flash('You are now logged in','success')
                    return redirect(url_for('dashboard'))
                else:
                    error='Invalid login'
                    return render_template('login.html',error=error)
            else:
                error='Username not found'
                return render_template('login.html',error=error)
        return render_template('login.html')

#check if the user is logged in
def is_logged_in(f):
    @wraps(f)
    def wrap(*args,**kwargs):
        if 'logged_in' in session:
            return f(*args,**kwargs)
        else:
            flash('unauthorized, please login ','danger')
            return redirect(url_for('login'))
    return wrap

#logout
@app.route('/logout')
def logout():
    session.clear()                                                                                   
    flash('you are logged out ','success')
    return redirect(url_for('login'))

#dashboard
@app.route('/dashboard',methods=['GET','POST'])
# @is_logged_in
def dashboard():

    brightness=request.form.get('bright')
    print brightness
    if request.method == 'POST':
        print "post"
        
        if request.form.get('switchA1') == 'ON':
            print "SwitchA1 ON"
            print request.form.get('switchA1')
            publish.single("hall/esp/gpio16", "1", hostname=my_ip)
            #publish.single("linga","hi", hostname=my_ip)
        elif request.form.get('switchA1') == 'OFF':
            print "switchA1 OFF"
            publish.single("hall/esp/gpio16", "0", hostname=my_ip)

        if request.form.get('switchA2') == 'ON':
            print "SwitchA2 ON"
            print request.form.get('switchA2')
            publish.single("hall/esp/gpio5", "1", hostname=my_ip)
        elif request.form.get('switchA2') == 'OFF':
            print "switchA2 OFF"
            publish.single("hall/esp/gpio5", "0", hostname=my_ip)

        if request.form.get('switchA3') == 'ON':
            print "SwitchA3 ON"
            print request.form.get('switchA3')
            publish.single("hall/esp/gpio4", "1", hostname=my_ip)
        elif request.form.get('switchA3') == 'OFF':
            print "switchA3 OFF"
            publish.single("hall/esp/gpio4", "0", hostname=my_ip)

        if request.form.get('switchA4') == 'ON':
            print "SwitchA4 ON"
            print request.form.get('switchA4')
            publish.single("hall/esp/gpio2", "1", hostname=my_ip)
        elif request.form.get('switchA4') == 'OFF':
            print "switchA4 OFF"
            publish.single("hall/esp/gpio2", "0", hostname=my_ip)

        if request.form.get('switchA5') == 'ON':
            print "SwitchA5 ON"
            print request.form.get('switchA5')
            publish.single("hall/esp/gpio14", "1", hostname=my_ip)
        elif request.form.get('switchA5') == 'OFF':
            print "switchA5 OFF"
            publish.single("hall/esp/gpio14", "0", hostname=my_ip)

        if request.form.get('switchA6') == 'ON':
            print "SwitchA6 ON"
            print request.form.get('switchA6')
            publish.single("hall/esp/gpio12", "1", hostname=my_ip)
        elif request.form.get('switchA6') == 'OFF':
            print "switchA6 OFF"
            publish.single("hall/esp/gpio12", "0", hostname=my_ip)

        if request.form.get('switchA7') == 'ON':
            print "SwitchA7 ON"
            print request.form.get('switchA5')
            publish.single("hall/esp/gpio13", "1", hostname=my_ip)
        elif request.form.get('switchA7') == 'OFF':
            print "switchA7 OFF"
            publish.single("hall/esp/gpio13", "0", hostname=my_ip)
        
        if request.form.get('switchA8') == 'ON':
            print "SwitchA8 ON"
            print request.form.get('switchA8')
            publish.single("hall/esp/gpio15", "1", hostname=my_ip)
        elif request.form.get('switchA8') == 'OFF':
            print "switchA8 OFF"
            publish.single("hall/esp/gpio15", "0", hostname=my_ip)
        elif request.form.get("rst")=="rstespconf":
            print "RESET ESP conference"
            publish.single("rst/esp/conf", "reset", hostname=my_ip)

        # elif request.form.get('toggleA1') == 'ON':
        #     print "toggleA1 is  ON"
        # elif request.form.get('toggleA1') == 'OFF':
        #     print "toggleA1 is  OFF"
        #if brightness:
            #publish.single("switch1", brightness, hostname="192.168.0.140")
        client.loop_start()

        def on_message(client, userdata, msg):
            print "Topic: ", msg.topic + "\nMessage: " + str(msg.payload)

            # bot = telepot.Bot('576910642:AAECcEM6oPOZYhHj8yM1Zhf1gu-5zeKdkBc')
            # def handle(msg):
            #     content_type, chat_type, chat_id = telepot.glance(msg)
            #     print(content_type, chat_type, chat_id)

            #     if message=="section-A switch1 is on":
            #         bot.sendMessage(chat_id, "bot_message: '{}'".format(message))
            # bot.message_loop(handle)

        client.on_connect = on_connect
        client.on_message = on_message
        client.connect(mqtt_broker_ip, 1883)

    return render_template('dashboard.html',brightness=brightness)
    client.loop_stop()

# @app.route('/ota_auth',methods=['GET','POST'])
# def ota_auth():                                                                               
#     if(request.method=='POST'):
#         # ota_username=request.form.get('username')
#         # ota_password=request.form.get('password')
#         if request.form.get('username')=='linga':
#             publish.single('linga','1', hostname=my_ip)
#     return render_template('ota_auth.html')

if __name__=='__main__':
    app.run(debug=True,host=my_ip)
