classdef SimpleDPP < handle
    properties
        putcharhandle;
        SimpleDPPRevErrorhandle;
        SimpleDPPRecvhandle;
        SimpleDPPRevState;
        SimpleDPPErrorCnt;
        send_buffer;
        recv_buffer;
        error_code;
    end
    properties(Constant,Hidden)
        SIMPLEDPP_REV_WAIT_START=0;
        SIMPLEDPP_REV_WAIT_END=1;
        SIMPLEDPP_REV_WAIT_CTRL_BYTE=2;
        SOH=cast(hex2dec('01'),'uint8');
        EOT=cast(hex2dec('04'),'uint8');
        ESC=cast(hex2dec('18'),'uint8');
        dtype = 'uint8'
        
        %define SimpleDPP receive error code
        %level 0:
        SIMPLEDPP_RECEIVE_ERROR=-1;
        SIMPLEDPP_SENDFAILED=-2; %USING,SEND ONLY USING THIS ERROR CODE
        SIMPLEDPP_NORMAL = 0;
        %level 1:
        SIMPLEDPP_ERROR_REV_OVER_CAPACITY=-11;
        SIMPLEDPP_ERROR_SEND_OVER_CAPACITY=-12;
        %level 2:
        SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END=-21;
        SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE=-22;
        SIMPLEDPP_CRC_CHECK_ERROR=-23;
    end
    
    methods
        function obj = SimpleDPP(putcharhandle,SimpleDPPRecvhandle,SimpleDPPRevErrorhandle)
            if(nargin == 3)
                obj.putcharhandle = putcharhandle;
                obj.SimpleDPPRecvhandle = SimpleDPPRecvhandle;
                obj.SimpleDPPRevErrorhandle = SimpleDPPRevErrorhandle;
                obj.send_buffer = mbuffer('uint8');
                obj.recv_buffer = mbuffer('uint8');
                obj.SimpleDPPRevState = obj.SIMPLEDPP_REV_WAIT_START;
                obj.error_code = obj.SIMPLEDPP_NORMAL;
                obj.SimpleDPPErrorCnt = 0;
            end
        end
        
        %data Ϊ'uint8'
        function flag = send(obj,data)
            if(isa(data,obj.dtype))
                obj.send_buffer.clear();
                obj.send_buffer.push(obj.SOH);
                len = length(data);
                for i = 1:len
                    c = data(i);
                    if(obj.containSimpleDPPCtrolByte(c))
                        obj.send_buffer.push(obj.ESC);
                        obj.send_buffer.push(c);
                    else
                        obj.send_buffer.push(c);
                    end
                    
                end
                obj.send_buffer.push(obj.EOT);
                obj.send_buffer_fun();
            else
                error("The input datatype is mismatching obj.dtype:",obj.dtype);
                flag = false;
            end
        end
        
        function parse(obj,datas)
            if(isa(datas,obj.dtype))
                len = length(datas);
                for i = 1:len
                    obj.parse_one_byte(datas(i));
                end
            else
                error("The input datatype is mismatching obj.dtype:",obj.dtype);
            end
        end
    end
    
    
    
    
    methods(Hidden)
        function flag = containSimpleDPPCtrolByte(obj,c)
            flag = ((c) == obj.SOH || (c) == obj.EOT || (c) == obj.ESC);
        end
        
        function parse_one_byte(obj,c)
            switch(obj.SimpleDPPRevState)
                case obj.SIMPLEDPP_REV_WAIT_START
                    if (c == obj.SOH)
                        obj.SimpleDPPRevState = obj.SIMPLEDPP_REV_WAIT_END;
                    end
                case obj.SIMPLEDPP_REV_WAIT_END
                    switch (c)
                        case obj.SOH
                            obj.SimpleDPPRevState = obj.SIMPLEDPP_REV_WAIT_START;
                            obj.SimpleDPPErrorInnerHandle(obj.SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END);
                        case obj.EOT
                            obj.SimpleDPPRevState = obj.SIMPLEDPP_REV_WAIT_START;
                            obj.SimpleDPPRecvInnerHandle();
                        case obj.ESC
                            obj.SimpleDPPRevState = obj.SIMPLEDPP_REV_WAIT_CTRL_BYTE;
                        otherwise
                            obj.recv_buffer.push(c);
                    end
                case obj.SIMPLEDPP_REV_WAIT_CTRL_BYTE
                    if (obj.containSimpleDPPCtrolByte(c))
                        obj.recv_buffer.push(c);
                        obj.SimpleDPPRevState = obj.SIMPLEDPP_REV_WAIT_END;
                    else
                        obj.SimpleDPPErrorInnerHandle(obj.SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE);
                    end
            end
        end
        
        function send_buffer_fun(obj)
            len = obj.send_buffer.length();
            if(~isempty(obj.putcharhandle))
                for i = 1:len
                    obj.putcharhandle(obj.send_buffer.buf(i));
                end
            end
        end
        
        function SimpleDPPRecvInnerHandle(obj)
            if(~isempty(obj.SimpleDPPRecvhandle))
                len = obj.recv_buffer.length;
                datas = obj.recv_buffer.buf(1:len);
                obj.SimpleDPPRecvhandle(datas);
            end
        end
        
        function SimpleDPPErrorInnerHandle(obj)
            obj.SimpleDPPErrorCnt = obj.SimpleDPPErrorCnt +1;
            if(~isempty(obj.SimpleDPPRevErrorhandle))
                obj.SimpleDPPRevErrorhandle(obj.error_code);
            end
        end
        
    end
end