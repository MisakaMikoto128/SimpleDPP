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
    ret = sdp.send("Hello")
    ret = sdp.send(b"Hello")
    ret = sdp.send_datas("Hello","World",encoding="utf-8")
    ret = sdp.send_datas("Hello","World")   #using default encoding="utf-8"
    ret = sdp.send_datas("Hello",b"World",encoding="utf-8") #str and bytes type mixing parameters