from subprocess import *
import threading
import time
import sys
from queue import Queue
import os
import time

web = sys.argv[1]

sem = threading.Semaphore(0)

record = True if len(sys.argv) == 3 and sys.argv[2]=="record" else False
FNULL = open('/dev/null', 'w')

def run_chrome():
    Popen(['chromium-browser', '--headless', '--remote-debugging-port=9222', '--ignore-certificate-errors', '--disable-gpu', '--user-data-dir=/tmp/nonexistent$(date +%s%N)', '--disk-cache-size=1'], stdout=FNULL, stderr=STDOUT)
    sem.acquire()
    call(['pkill', 'chromium'])

def filter(temp, delay):
    ttfb = open(temp, 'r').read().split('\n')
    new_ttfb = []
    while ttfb[-1] == '':
        del ttfb[-1]
    for t in ttfb:
        if float(t.split('\t')[1]) / delay > 0.01:
            new_ttfb.append(t)
    tmp = open(temp, 'w+')
    for t in new_ttfb:
        tmp.write(t + '\n')
    tmp.close()


chrome = threading.Thread(target=run_chrome)
chrome.start()

temp = open('tmp','w+')
time.sleep(2)
begin = time.time()
if record:
    call(['node', 'run.js', web, 'true'], stdout=temp)
else:
    call(['node', 'run.js', web, 'false'])
end = time.time()

if record:
    filter('tmp', end-begin)

stage = 'record' if record else 'replay'
lastslash = web.rfind('/')
time_collection = open(os.path.join('plTime', web[lastslash+5:]), 'a')
time_collection.write("{}\t{}\n".format(stage, str(end-begin)))
time_collection.close()

if record:
    time.sleep(1)

sem.release()