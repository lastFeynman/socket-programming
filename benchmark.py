import urllib.request
import time
import threading


class ResultThread(threading.Thread):
    def __init__(self,func,args=()):
        super(ResultThread,self).__init__()
        self.func = func
        self.args = args
    def run(self):
        self.result = self.func(*self.args)
    def get_result(self):
        try:
            return self.result
        except Exception:
            return None


def send_request(tid, url):
    try:
        response = urllib.request.urlopen(url)
        print('request ' + str(tid) + ' has finished: ' + str(response.getcode()))
    except Exception:
        print('request ' + str(tid) + ' went wrong.')
        return -1;

    return 0;


if __name__ == "__main__":
    url = 'http://192.168.44.128:8080'
    n = 10000
    threads = []
    error = 0

    for i in range(n):
        threads.append(ResultThread(send_request, args=(i, url)))

    start = time.perf_counter()

    for i in range(n):
        threads[i].start()

    for i in range(n):
        threads[i].join()
        if threads[i].get_result() == -1:
            error += 1

    end = time.perf_counter()
    print(str(error) + ' threads went wrong.')
    print('total time: ' + str(end - start))

