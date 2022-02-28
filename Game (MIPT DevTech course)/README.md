#How to install?
##(for Linux)

This is a game for 2 computers.

1) clone repository on both of them\
`git clone https://github.com/maljunaplanedo/tpgame`
   
2) install PyGObject and pycairo for Linux\n
`sudo apt-get install pkg-config`\
   `sudo apt-get install libcairo2-dev`\
   `sudo apt install python3-gi python3-gi-cairo gir1.2-gtk-3.0`\
   `sudo apt install libgirepository1.0-dev gcc libcairo2-dev pkg-config python3-dev gir1.2-gtk-3.0`
   
3) install Python modules
   `pip install -r requirements.txt`
 
4) on host computer, type this\
`host PORT`
   to netconfig.txt, where PORT is any port

5) on other computer, type this\
`ADDRESS PORT`
   to netconfig.txt, where PORT is the por t 
   you've specified for the host computer, 
   ADDRESS is the address of the host computer 
   in your local network. 

5) run main.py
`python3 main.py`