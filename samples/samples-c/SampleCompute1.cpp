#include "SampleBase.hpp"

class SampleCompute1: public ComputeSampleBase
{
public:
    int run(int argc, const char **argv)
    {
		// Create the programs.
		auto computeShader = compileShaderFromFile("data/shaders/computeAdd.glsl", AGPU_COMPUTE_SHADER);
		if (!computeShader)
			return false;

		// Create the shader signature.
		auto shaderSignatureBuilder = agpuCreateShaderSignatureBuilder(device);
		agpuBeginShaderSignatureBindingBank(shaderSignatureBuilder, 1);
		agpuAddShaderSignatureBindingBankElement(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);
		agpuAddShaderSignatureBindingBankElement(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);
		agpuAddShaderSignatureBindingBankElement(shaderSignatureBuilder, AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER, 1);
		auto shaderSignature = agpuBuildShaderSignature(shaderSignatureBuilder);
		agpuReleaseShaderSignatureBuilder(shaderSignatureBuilder);
		if (!shaderSignature)
			return false;

		// Create the compute pipeline builder
		auto pipelineBuilder = agpuCreateComputePipelineBuilder(device);
		agpuSetComputePipelineShaderSignature(pipelineBuilder, shaderSignature);
		agpuAttachComputeShader(pipelineBuilder, computeShader);

		// Build the pipeline
		auto pipeline = buildComputePipeline(pipelineBuilder);
		agpuReleaseComputePipelineBuilder(pipelineBuilder);
		if (!pipeline)
			return false;

		// Create the command allocator and the command list
		auto commandAllocator = agpuCreateCommandAllocator(device, AGPU_COMMAND_LIST_TYPE_DIRECT, commandQueue);
		auto commandList = agpuCreateCommandList(device, AGPU_COMMAND_LIST_TYPE_DIRECT, commandAllocator, pipeline);

		// Create different buffers for uploading, computing on the gpu, and reading back the
		agpu_size arraySize = 256;
		agpu_size arrayByteSize = arraySize* sizeof(float);
        agpu_size bufferByteSize = arrayByteSize * 3;
		auto uploadBuffer = createMappableUploadBuffer(bufferByteSize, nullptr);
        auto readbackBuffer = createMappableReadbackBuffer(bufferByteSize, nullptr);
        auto storageBuffer = createStorageBuffer(bufferByteSize, sizeof(float), nullptr);

		// Create the shader bindings
		auto shaderBindings = agpuCreateShaderResourceBinding(shaderSignature, 0);
		agpuBindStorageBufferRange(shaderBindings, 0, storageBuffer, 0, arrayByteSize);
		agpuBindStorageBufferRange(shaderBindings, 1, storageBuffer, arrayByteSize, arrayByteSize);
		agpuBindStorageBufferRange(shaderBindings, 2, storageBuffer, arrayByteSize*2, arrayByteSize);

		// Write the inputs
		auto mappedPointer = (float*)agpuMapBuffer(uploadBuffer, AGPU_WRITE_ONLY);
		float *leftInput = mappedPointer;
		float *rightInput = mappedPointer + arraySize;

		for (agpu_size i = 0; i < arraySize; ++i)
		{
			leftInput[i] = 1.0f + float(i);
			rightInput[i] = 1.0f + float(i) *2;
		}

		// Unmap the buffer.
		agpuUnmapBuffer(uploadBuffer);

        // Copy the data from the upload buffer
        {
            agpuPushBufferTransitionBarrier(commandList, storageBuffer, AGPU_STORAGE_BUFFER, AGPU_COPY_DESTINATION_BUFFER);
            agpuCopyBuffer(commandList, uploadBuffer, 0, storageBuffer, 0, bufferByteSize);
            agpuPopBufferTransitionBarrier(commandList);
	    }

		// Dispatch the compute
		agpuSetShaderSignature(commandList, shaderSignature);
		agpuUseComputeShaderResources(commandList, shaderBindings);
		agpuDispatchCompute(commandList, arraySize, 1, 1);

        // Copy the data into the readback buffer.
        {
            agpuPushBufferTransitionBarrier(commandList, storageBuffer, AGPU_STORAGE_BUFFER, AGPU_COPY_SOURCE_BUFFER);
            agpuCopyBuffer(commandList, storageBuffer, 0, readbackBuffer, 0, bufferByteSize);
            agpuPopBufferTransitionBarrier(commandList);
        }

		agpuCloseCommandList(commandList);
		agpuAddCommandList(commandQueue, commandList);
		agpuFinishQueueExecution(commandQueue);

		// Map the readback buffer;
        mappedPointer = (float*)agpuMapBuffer(readbackBuffer, AGPU_READ_ONLY);
        leftInput = mappedPointer;
        rightInput = mappedPointer + arraySize;
		float *output = rightInput + arraySize;

		// Readback the output
		int exitCode = 0;
		for (size_t i = 0; i < arraySize; ++i)
		{
			auto left = leftInput[i];
			auto right = rightInput[i];
			auto result = output[i];
			auto expected = left + right;
			if (expected != result)
			{
				fprintf(stderr, "%d: %f + %f = %f\n", (int)i, left, right, result);
				exitCode = 1;
			}
            else if(left == 0 && right == 0 && expected == 0)
            {
                fprintf(stderr, "%d is completely zero\n", (int)i);
                exitCode = 1;
            }
		}

        agpuUnmapBuffer(readbackBuffer);
		if (exitCode == 0)
			printf("Success\n");

		agpuReleaseCommandList(commandList);
		agpuReleaseCommandAllocator(commandAllocator);
		agpuReleasePipelineState(pipeline);
		agpuReleaseShaderResourceBinding(shaderBindings);
		agpuReleaseShaderSignature(shaderSignature);
		agpuReleaseBuffer(uploadBuffer);
        agpuReleaseBuffer(storageBuffer);
        agpuReleaseBuffer(readbackBuffer);

        return exitCode;
    }
};

SAMPLE_MAIN(SampleCompute1)
