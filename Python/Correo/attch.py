#!/usr/bin/python
import time
import smtplib,ssl
from email.mime.multipart import MIMEMultipart
from email.mime.base import MIMEBase
from email.mime.text import MIMEText           
from email.utils import COMMASPACE, formatdate
from email import encoders 

import datetime

send_from = 'meteorologicafenixcatie@gmail.com'
send_to = 'meteorologicafenixcatie@gmail.com'
subject = 'PRUEBA'
text = 'Datos del mes de JULIO'
username='meteorologicafenixcatie@gmail.com'
password='fenixcatie'
isTls=True

msg = MIMEMultipart()
msg['From'] = send_from
msg['To'] = send_to
msg['Date'] = formatdate(localtime = True)
msg['Subject'] = subject
msg.attach (MIMEText(text))

part = MIMEBase('application', "octet-stream")
part.set_payload(open("Prueba.txt", "rb").read())
encoders.encode_base64(part)
part.add_header('Content-Disposition', 'attachment; filename="Prueba.txt"')
msg.attach(part)

#context = ssl.SSLContext(ssl.PROTOCOL_SSLv3)
#SSL connection only working on Python 3+
smtp = smtplib.SMTP('smtp.gmail.com',587)
if isTls:
    smtp.starttls()
smtp.login(username,password)
smtp.sendmail(send_from, send_to, msg.as_string())
smtp.quit()
