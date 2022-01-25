classdef mbuffer < handle
    properties
        mlength;
        buf;
        capacity;
        dtype;
    end
    properties(Constant)
        block_size = 1024;
    end
    
    methods
        function obj = mbuffer(dtype,initila_capacity)
            if(nargin == 1)
                obj.capacity = obj.block_size;
            else
                obj.capacity = initila_capacity;
            end
            obj.mlength = 0;
            obj.dtype = dtype;
            obj.buf = zeros(1,obj.capacity,dtype);
        end
        function flag = push(obj,data)
            if(isa(data,obj.dtype))
                len = length(data);
                if(obj.mlength + len > obj.capacity)
                    obj.buf = horzcat(obj.buf,zeros(1,obj.block_size));
                    obj.capacity = obj.capacity + obj.block_size;
                end
                for i = 1:len
                    obj.buf(obj.mlength+i) = data(i);
                end
                obj.mlength = obj.mlength + len;
                flag = true;
            else
                error("The input datatype is mismatching obj.dtype:",obj.dtype);
                flag = false;
            end
        end
        function len = length(obj)
            len = obj.mlength;
        end
        function flag = pop(obj)
            if(obj.mlength >= 1)
                obj.mlength = obj.mlength - 1;
                flag = true;
            else
                error("mbuffer is empty");
                flag = false;
            end
            
        end
        function clear(obj)
            obj.mlength = 0;
        end
    end
end