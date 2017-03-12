# Project Title

An socket server which can show one random JPG image out of 5 given images.

## Installing

make RandomPic.cpp

## Running the tests

1. Run program: ./RandomPic.out
2. Open "http://yourIP/test.jpg" with browser
3. F5 for refresh webpage

## Deployment

### Source code structure

Image- Given JPG image

### Program achitecture(I am not sure what it is. I wrote down the flow in here)

Main
- Build connection
-- socket
-- bind
-- listen
- SendPic
-- while 1
--- accept
---- send header
---- send picture binary
---- shuffle image and replace test.jpg 
- Close connection
    
## Authors

* **Daniel Wang** - *Initial work* - [RandomPic](https://github.com/)
