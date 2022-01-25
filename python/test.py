class Person:
    def say_hi(self,name):
        print("hello I am " ,self.name)
    def say_hi(self,name,age):
        print("hello I am{0} age{1}",format(name,age))