import subprocess
import os
import signal
import time

prozess = subprocess.Popen("make run", stdout=subprocess.PIPE, shell=True, preexec_fn=os.setsid, text=True)
time.sleep(3)
subprocess.run(["firefox", "~/Documents/programming/JanGeschenk/web/index.html"])
time.sleep(20)
os.killpg(os.getpgid(prozess.pid), signal.SIGTERM)
    

