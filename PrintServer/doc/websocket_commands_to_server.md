# Websocket commands

Here you will find a list of all the websocket commands that can be send from the client (webbrowser) to the server (esp). These commands are optimised to be as small as possible and lightweight for the server.

 > format: {id};{parameter} (multiple parameters are seperated with a ';')

| Command id | Parameters |  format | range  |
|  --------  |   -------  | ------- | ------ |
| 00  |  (delete file)   | (string) | 0-128bytes |
| 01  |  (print file)   | (string) | 0-128bytes |