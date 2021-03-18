import argparse
import os
import select
import serial
import sys
import time
import tty

# TODO: Make ti work with contexts (with UartConnection() as u)


class UartConnection:

    def __init__(self, file_path, baud_rate):
        self.serial = serial.Serial(file_path, baud_rate)

    def send_line(self, line):
        if not line.endswith("\n"):
            # Intentionally not adding the \r for now
            line += "\n"
        return self.send_string(line)

    def send_string(self, string):
        return self.send_bytes(bytes(string, "ascii"))

    def send_bytes(self, bytes_to_send):
        return self.serial.write(bytes_to_send)

    def send_int(self, number):
        if number > 2 ** 32 - 1:
            raise 'Number can only be 4 bytes long'
        number_in_bytes = number.to_bytes(4, byteorder='big')
        return self.send_bytes(number_in_bytes)

    def read(self, max_len):
        return self.serial.read(max_len)

    def read_buffer(self):
        return self.read(self.serial.in_waiting)

    def read_buffer_string(self):
        return self._decode_bytes(self.read_buffer())

    def read_line(self):
        return self._decode_bytes(self.serial.readline())

    def read_int(self):
        bytes_to_read = 4
        number_bytes = self.read(bytes_to_read)
        return int.from_bytes(number_bytes, byteorder='big')

    def start_interactive(self, input_file, output_file):
        try:
             # Make the tty cbreak
             # https://www.oreilly.com/library/view/python-standard-library/0596000960/ch12s08.html
            tty.setcbreak(input_file.fileno())
            while True:
                rfd, _, _ = select.select([self.serial, input_file], [], [])

                if self.serial in rfd:
                    r = self.read_buffer_string()
                    output_file.write(r)
                    output_file.flush()

                if input_file in rfd:
                    r = input_file.read(1)
                    self.send_string(r)
        except KeyboardInterrupt:
            print("Got keyboard interrupt. Terminating...")
            return
        except OSError as e:
            print("Got OSError. Terminating...")
            return
        finally:
            os.system("stty sane")

    def close(self):
        self.serial.close()

    def _decode_bytes(self, bytes_to_decode):
        return bytes_to_decode.decode("ascii")


def compute_kernel_checksum(kernel_bytes):
    num = 0
    for b in kernel_bytes:
        num = (num + b) % (2 ** 32)
    return num

def send_kernel_debug(uart_connection, kernel):
    for i, b in enumerate(kernel):
        print(i, 'Sending byte', b)
        uart_connection.send_bytes(bytes([b]))
        read_byte = uart_connection.read(1)[0]
        print(i, 'Received byte', read_byte)

        if b != read_byte:
            print(i, 'Sent', b, 'but got', read_byte)
            return False

    return True


def send_kernel(path, uart_connection, debug=False):
    with open(path, mode='rb') as f:
        uart_connection.send_line("kernel")
        time.sleep(1)

        kernel = f.read()
        size = len(kernel)
        checksum = compute_kernel_checksum(kernel)

        print("Sending kernel with size", size, "and checksum", checksum)
        uart_connection.send_int(size)
        time.sleep(1)
        size_confirmation = uart_connection.read_int()

        if size_confirmation != size:
            print("Expected size to be", size, "but got", size_confirmation)
            return False

        print("Kernel size confirmed. Sending kernel")

        #if debug:
            #print("Starting debug workflow")
            #uart_connection.send_int(1)
            #send_kernel_debug(uart_connection, kernel)
        #else:
            #uart_connection.send_int(0)
        
        for i in range(size):
            uart_connection.send_bytes(i)
            ser.flush()
            if i%100==0:
                print("{:>6}/{:>6} bytes".format(i, length))
        #print("Validating checksum...")
        #checksum_confirmation = uart_connection.read_int()
        #if checksum_confirmation != checksum:
            #print("Expected checksum to be", checksum,
                  #"but was", checksum_confirmation)
            #return False

        #line = uart_connection.read_line()
        #print("Received: ", line)
        #if not line.startswith("Done"):
            #print("Didn't get confirmation for the kernel. Got", line)
            #return False

        return True




uart_connection = UartConnection('/dev/ttyUSB0', 115200)
time.sleep(1)
    
send_kernel('kernel9.img',uart_connection, False)

time.sleep(1)

uart_connection.close()



