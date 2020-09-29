# File Downloader
Download file from the unstable (packet-loss) server using UDP packets, written in C. Program sends request to get parts of the file to the server, server sends particular chunk of the file in response. 

### Transport server
There is a server binary file in repository named **transport-server**. To run the server type the following command: ```./transport-server PORT```, where ```PORT``` is the port number server listens on, eg. ```./transport-server 7777```.
###### Rules of server
* response is sent with probability of $50\%$
* every response will be duplicated with  probability of $20\%$ (this duplicate could also be duplicated with the same probability)
* every response is sent with the delay between $0.5$ and $1.5$ seconds
* server keeps queue of maximum of 1000 datagrams to send

### Usage of the program
To run the file downloader type the command: ```./transport IP PORT OUTPUT_FILE BYTES```, where ```PORT``` and ```IP``` are port and IP address of listening transport server, ```OUTPUT_FILE``` is the destination of the downloaded file and ```BYTES``` is the number of bytes to download from the server.

### Example usage
```./transport 172.217.16.14 77 output 1000``` will download ```1000``` bytes of the file from the server listening on port ```77```, IP address ```172.217.16.14``` and save it in the file named ```output```.

### Download file
In order to download the file you need to do these things:
1) Run the transport server, eg. ```./transport-server 7777```
2) Download the file with transport program, eg. (if the server is running on the same computer) ```./transport 7777 127.0.0.1 output 1000000```.

### Example output
```
Done 10.00%
Done 20.00%
Done 30.00%
Done 40.00%
Done 50.00%
Done 60.00%
Done 70.00%
Done 80.00%
Done 90.00%
Done 100.00%
```