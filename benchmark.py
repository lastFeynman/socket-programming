import urllib.request
import time
import threading


def send_request(tid, url):
    with urllib.request.urlopen(url) as response:
        print('request ' + str(tid) + ' has finished: ' + str(response.getcode()))


if __name__ == "__main__":
    url = '192.168.44.128:8080'
    n = 10000
    threads = []

    for i in range(n):
        threads.append(threading.Thread(target=send_request, args=(i, url)))

    start = time.perf_counter()

    for i in range(n):
        threads[i].start()

    for i in range(n):
        threads[i].join()

    end = time.perf_counter()

    print('total time: ' + str(end - start))

