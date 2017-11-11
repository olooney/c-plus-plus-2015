import sys
import socket
import unittest
import subprocess
import time

class TestAsioServer(unittest.TestCase):
    def test_everything(self):
        server_process = subprocess.Popen("./server")
        time.sleep(0.1)

        # check if the server is waiting and didn't just crash on start-up
        self.assertIsNone(server_process.poll())

        # connect
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect( ('localhost', 8181) )

        # helper method for common sock.send()/sock.recv() pair
        def send_recv(query):
            sock.send(query)
            answer = sock.recv(1024)
            return answer

        self.assertEqual(send_recv(b'GET'), b'GOT 42\n')
        self.assertEqual(send_recv(b'SET 6*7'), b'SAT\n')
        self.assertEqual(send_recv(b'GET'), b'GOT 6*7\n')

        # this should tell the server to exit gracefully
        sock.send(b'DIE')
        sock.close()
        time.sleep(0.1)

        # make sure the child process exited when it was told to
        self.assertEqual(server_process.poll(), 0)

if __name__ == '__main__':
    unittest.main()
