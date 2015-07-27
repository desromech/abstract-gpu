# Abstract gpu shader optimization

# Function pass. Used for optimization
class FunctionPass:
    def runOnFunction(self, function, passManager):
        pass

# Function pass manager. Used for running optimization passes
class FunctionPassManager:
    def __init__(self, passes = []):
        self.passes = passes

    def addPass(self, functionPassClass):
        self.passes.append(functionPassClass)
        
    def runOnFunction(self, function):
        for functionPassClass in self.passes:
            functionPass = functionPassClass()
            functionPass.runOnFunction(function, self)

# Converts allocas into registers.
class PromoteAllocaToRegisters:
    def runOnFunction(self, function, passManager):
        #print function.getName(), 
        #print function.computeDominance()
        function.computeDominance()
        #print function
