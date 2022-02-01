function test(varargin)
    disp("Number of input arguments: " + nargin)
    for i = 1:nargin
        disp(varargin(i))
    end
end