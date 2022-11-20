#include <fltkernel.h>

#define log(format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "kernel_user_communication!" __FUNCTION__ ": " format, __VA_ARGS__)

namespace globals
{
	UNICODE_STRING device_name   = RTL_CONSTANT_STRING(L"\\Device\\kernel_user_communication");
	UNICODE_STRING symbolic_link = RTL_CONSTANT_STRING(L"\\??\\kernel_user_communication");
}

EXTERN_C_START
DRIVER_DISPATCH	  DriverDispatch;
DRIVER_DISPATCH	  DriverRead;
DRIVER_DISPATCH	  DriverWrite;
DRIVER_DISPATCH	  DriverCreate;
DRIVER_DISPATCH	  DriverClose;
DRIVER_UNLOAD	  DriverUnload;
DRIVER_INITIALIZE DriverEntry;
EXTERN_C_END

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, DriverDispatch)
#pragma alloc_text(PAGE, DriverRead)
#pragma alloc_text(PAGE, DriverWrite)
#pragma alloc_text(PAGE, DriverCreate)
#pragma alloc_text(PAGE, DriverClose)
#pragma alloc_text(PAGE, DriverUnload)
#pragma alloc_text(INIT, DriverEntry)
#endif

auto DriverDispatch(PDEVICE_OBJECT device_object, PIRP irp) -> NTSTATUS
{
	UNREFERENCED_PARAMETER(device_object);

	auto ntstatus = STATUS_SUCCESS;
	auto stack = IoGetCurrentIrpStackLocation(irp);

	irp->IoStatus.Information = 0ul;

	auto buffer		   = irp->AssociatedIrp.SystemBuffer;
	auto buffer_length = stack->Parameters.DeviceIoControl.InputBufferLength;
	auto ioctl		   = stack->Parameters.DeviceIoControl.IoControlCode;

	{
		UNREFERENCED_PARAMETER(buffer);
		UNREFERENCED_PARAMETER(buffer_length);

		log("ioctl command (%x)\n", ioctl);

		irp->IoStatus.Information = 0ul;
		irp->IoStatus.Status = STATUS_SUCCESS;
	}

	irp->IoStatus.Status = ntstatus;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

auto DriverRead(PDEVICE_OBJECT device_object, PIRP irp) -> NTSTATUS
{
	UNREFERENCED_PARAMETER(device_object);

	auto ntstatus = STATUS_SUCCESS;
	auto stack = IoGetCurrentIrpStackLocation(irp);

	irp->IoStatus.Information = 0ul;

	auto buffer		   = irp->UserBuffer;
	auto buffer_length = stack->Parameters.DeviceIoControl.InputBufferLength;

	{
		UNREFERENCED_PARAMETER(buffer);
		UNREFERENCED_PARAMETER(buffer_length);

		log("read request\n");

		irp->IoStatus.Information = 0ul;
		irp->IoStatus.Status = STATUS_SUCCESS;
	}

	irp->IoStatus.Status = ntstatus;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

auto DriverWrite(PDEVICE_OBJECT device_object, PIRP irp) -> NTSTATUS
{
	UNREFERENCED_PARAMETER(device_object);

	auto ntstatus = STATUS_SUCCESS;
	auto stack	  = IoGetCurrentIrpStackLocation(irp);

	irp->IoStatus.Information = 0ul;

	auto buffer		   = irp->UserBuffer;
	auto buffer_length = stack->Parameters.DeviceIoControl.OutputBufferLength;

	{
		UNREFERENCED_PARAMETER(buffer);
		UNREFERENCED_PARAMETER(buffer_length);

		log("write request\n");

		irp->IoStatus.Information = 0ul;
		irp->IoStatus.Status	  = STATUS_SUCCESS;
	}

	irp->IoStatus.Status = ntstatus;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

auto DriverCreate(PDEVICE_OBJECT device_object, PIRP irp) -> NTSTATUS
{
	UNREFERENCED_PARAMETER(device_object);

	irp->IoStatus.Status	  = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	log("device opened\n");
	return STATUS_SUCCESS;
}

auto DriverClose(PDEVICE_OBJECT device_object, PIRP irp) -> NTSTATUS
{
	UNREFERENCED_PARAMETER(device_object);

	irp->IoStatus.Status	  = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	log("device closed\n");
	return STATUS_SUCCESS;
}

auto DriverUnload(PDRIVER_OBJECT driver_object) -> void
{
	IoDeleteSymbolicLink(&globals::symbolic_link);
	IoDeleteDevice(driver_object->DeviceObject);

	log("unloaded\n");
}

auto DriverEntry(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path) -> NTSTATUS
{
	UNREFERENCED_PARAMETER(registry_path);

	NTSTATUS ntstatus			 = STATUS_SUCCESS;
	PDEVICE_OBJECT device_object = nullptr;

	ntstatus = IoCreateDevice(driver_object, 0, &globals::device_name, FILE_DEVICE_UNKNOWN, 0, FALSE, &device_object);
	if (!NT_SUCCESS(ntstatus)) return ntstatus;

	ntstatus = IoCreateSymbolicLink(&globals::symbolic_link, &globals::device_name);
	if (!NT_SUCCESS(ntstatus)) goto lcleanup_device;

	driver_object->Flags |= DO_BUFFERED_IO;
	driver_object->DriverUnload = DriverUnload;

	driver_object->MajorFunction[IRP_MJ_CREATE] = DriverCreate;
	driver_object->MajorFunction[IRP_MJ_CLOSE]  = DriverClose;
	
	driver_object->MajorFunction[IRP_MJ_READ]   = DriverRead;
	driver_object->MajorFunction[IRP_MJ_WRITE]  = DriverWrite;

	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatch;

	log("loaded (DO_BUFFERED_IO)\n");

	return ntstatus;

//lcleanup_link:
	//IoDeleteSymbolicLink(&globals::symbolic_link);

lcleanup_device:
	IoDeleteDevice(device_object);
	device_object = nullptr;

	return ntstatus;
}