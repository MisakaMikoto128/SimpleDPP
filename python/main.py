from SimpleDPP import SimpleDPP

sdp = SimpleDPP()

def SimpleDPPRevErrorCallback(errorCode: int):
    print("_SimpleDPPRevErrorCallback: errorCode = " + str(errorCode))

def SimpleDPPReceiveCallback(data: bytes):
    print("SimpleDPPReceiveCallback: data = "+str(bytes(data)))

def SimpleDPPSendBytesData(data: bytes):
    print(data)
    print(type(data))
    sdp.parse(data)

if __name__ == "__main__":
    sdp.setCallback(SimpleDPPReceiveCallback,SimpleDPPRevErrorCallback,SimpleDPPSendBytesData)
    sdp.send("Hello")