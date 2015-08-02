#include "command_queue.hpp"
#include "command_list.hpp"

_agpu_command_queue::_agpu_command_queue()
{
	
}

agpu_command_queue *_agpu_command_queue::create(agpu_device *device)
{
	auto queue = new _agpu_command_queue();
	queue->device = device;
	return queue;
}

void _agpu_command_queue::lostReferences()
{
}

agpu_error _agpu_command_queue::addCommandList ( agpu_command_list* command_list )
{
	CHECK_POINTER(command_list);
	
	// TODO: Use multi-threading
	command_list->execute();
	return AGPU_OK;
}

AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue )
{
	CHECK_POINTER(command_queue);
	return command_queue->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue )
{
	CHECK_POINTER(command_queue);
	return command_queue->release();
}

AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list )
{
	CHECK_POINTER(command_queue);
	return command_queue->addCommandList(command_list);
}
