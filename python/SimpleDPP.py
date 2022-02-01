from functools import singledispatch, singledispatchmethod


class SimpleDPP(object):

    # define SimpleDPP receive error code
    # level 0:
    SIMPLEDPP_RECEIVE_ERROR = -1
    SIMPLEDPP_SENDFAILED = -2 #USING,SEND ONLY USING THIS ERROR CODE
    SIMPLEDPP_NORMAL = 0
   # level 1:
    SIMPLEDPP_ERROR_REV_OVER_CAPACITY = -11
    SIMPLEDPP_ERROR_SEND_OVER_CAPACITY = -12
   # level 2:
    SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END = -21
    SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE = -22
    SIMPLEDPP_CRC_CHECK_ERROR = -23
   # SimpleDPP receive state machine's states
    SIMPLEDPP_REV_WAIT_START = 0
    SIMPLEDPP_REV_WAIT_END = 1
    SIMPLEDPP_REV_WAIT_CTRL_BYTE = 2

   # SimpleDPP frame control byte (The frame delimiter)
    SOH = 0x01  # DEC: 1
    EOT = 0x04  # DEC: 4
    ESC = 0x18  # DEC: 27

    def __init__(self, SimpleDPPRevErrorCallback=None, simpleDPPSendBytesData=None, simpleDPPReceiveCallback=None):

        self.revBuffer = []
        self.sendBuffer = []
        self._SimpleDPPRevState = self.SIMPLEDPP_REV_WAIT_START
        self._SimpleDPPErrorCnt = 0

        self._SimpleDPPRevErrorCallback = SimpleDPPRevErrorCallback
        self._simpleDPPSendBytesData = simpleDPPSendBytesData
        self._simpleDPPReceiveCallback = simpleDPPReceiveCallback


    '''
    @brief: Simple DPP receive msg.SimpleDPP receive state machine's states
    @param c byte data you received. uint8_t type.
    '''
    @singledispatchmethod
    def parse(self, c:int):
        if self._SimpleDPPRevState == self.SIMPLEDPP_REV_WAIT_START:
            if c == self.SOH:
                self._SimpleDPPRevState = self.SIMPLEDPP_REV_WAIT_END
        elif self._SimpleDPPRevState == self.SIMPLEDPP_REV_WAIT_END:
            if c == self.SOH:
                self._SimpleDPPRevState = self.SIMPLEDPP_REV_WAIT_START
                self.SimpleDPPRevErrorInnerCallback(
                    self.SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END)
            elif c == self.EOT:
                self._SimpleDPPRevState = self.SIMPLEDPP_REV_WAIT_START
                self.SimpleDPPRecvInnerCallback()
            elif c == self.ESC:
                self._SimpleDPPRevState = self.SIMPLEDPP_REV_WAIT_CTRL_BYTE
            else:
                self.revBuffer.append(c)
        elif self._SimpleDPPRevState == self.SIMPLEDPP_REV_WAIT_CTRL_BYTE:
            if self.containSimpleDPPCtrolByte(c):
                self.revBuffer.append(c)
                self._SimpleDPPRevState = self.SIMPLEDPP_REV_WAIT_END
            else:
                self.SimpleDPPRevErrorInnerCallback(
                    self.SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE)
        else:
            pass

    @parse.register(bytes)
    @parse.register(list)
    def _(self,c):
        for datum in c:
            self.parse(datum)

    '''
    @brief: Simple DPP send msg.
    @param data: bytes data you will be sent.
    @return success: send data bytes length
            fail: SIMPLEDPP_SENDFAILED
    '''
    @singledispatchmethod
    def send(self, data:bytes)->int:
        self.sendBuffer.clear()
        self.sendBuffer.append(self.SOH)
        for datum in data:
            if self.containSimpleDPPCtrolByte(datum):
                self.sendBuffer.append(self.ESC)
                self.sendBuffer.append(datum)
            else:
                self.sendBuffer.append(datum)
        self.sendBuffer.append(self.EOT)
        self._simpleDPPSendBytesData(self.sendBuffer)
        return self.sendBuffer.__len__()

    @send.register
    def _(self,data:str,encoding='utf-8'):
        return self.send(data.encode(encoding))


    def send_datas_start(self):
        self.sendBuffer.clear()
        self.sendBuffer.append(self.SOH)

    def send_datas_add(self,data:bytes):
        for datum in data:
            if self.containSimpleDPPCtrolByte(datum):
                self.sendBuffer.append(self.ESC)
                self.sendBuffer.append(datum)
            else:
                self.sendBuffer.append(datum)

    def send_datas_end(self):
        self.sendBuffer.append(self.EOT)
        self._simpleDPPSendBytesData(self.sendBuffer)

    def send_datas(self,*datas,**charset):
        """
        :type datas: list bytes
        :charset : only accept encoding = 'xx coding'
        """
        self.send_datas_start()
        if(len(datas) > 0):
            if(isinstance(datas[0],str)):
                if(len(charset) > 0):
                    for data in datas:
                        self.send_datas_add(data.encode(charset['encoding']))
                else:
                    for data in datas:
                        self.send_datas_add(data.encode('utf-8'))
            elif(isinstance(datas[0],bytes)):
                for data in datas:
                    self.send_datas_add(data)
        self.send_datas_end()

    @classmethod
    def containSimpleDPPCtrolByte(cls, c):
        return (c == cls.SOH or c == cls.EOT or c == cls.ESC)

    @property
    def receiveErrorCnt(self):
        return self._SimpleDPPErrorCnt

    def setSimpleDPPRevErrorCallback(self, SimpleDPPRevErrorCallback):
        self._SimpleDPPRevErrorCallback = SimpleDPPRevErrorCallback

    def setSimpleDPPSendBytesData(self, simpleDPPSendBytesData):
        self._simpleDPPSendBytesData = simpleDPPSendBytesData

    def setSimpleDPPReceiveCallback(self, simpleDPPReceiveCallback):
        self._simpleDPPReceiveCallback = simpleDPPReceiveCallback

    def setCallback(self, simpleDPPReceiveCallback, simpleDPPRevErrorCallback, simpleDPPSendBytesData):
        self.setSimpleDPPReceiveCallback(simpleDPPReceiveCallback)
        self.setSimpleDPPRevErrorCallback(simpleDPPRevErrorCallback)
        self.setSimpleDPPSendBytesData(simpleDPPSendBytesData)


    def SimpleDPPRecvInnerCallback(self):
        if self._simpleDPPReceiveCallback != None:
            self._simpleDPPReceiveCallback(self.revBuffer)
            self.revBuffer.clear()

    def SimpleDPPRevErrorInnerCallback(self, errorCode: int):
        self._SimpleDPPErrorCnt += 1
        if self._SimpleDPPRevErrorCallback != None:
            self._SimpleDPPRevErrorCallback(errorCode)
