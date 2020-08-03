/*
IOCTL����
*/
#pragma warning (disable : 4100 4995 4002)

/* ���˳�����,��Ȼ�ᱨ�� */
#include <ntifs.h>
#include <ntddk.h>

/* ��������,��Ȼ�ᱨ�� */
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

/* ���ݽṹ */
struct DataStruct
{
	HANDLE PID;			//����ID
	PVOID64 Addr;		//��ַ
	PVOID64 Result;		//���
	SIZE_T Size;				//��С
};

/* ��ȡ */
#define IOCTL_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

/* д�� */
#define IOCTL_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x998, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

/* ���Ը������� */
#define DebugMessage(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

/* �豸���� */
UNICODE_STRING g_DeviceName = RTL_CONSTANT_STRING(L"\\Device\\Driver_IOCTL");

/* �豸���ӷ��� */
UNICODE_STRING g_SymbolicLink = RTL_CONSTANT_STRING(L"\\DosDevices\\Driver_IOCTL");

/* �豸����ָ�� */
PDEVICE_OBJECT g_DeviceObject;

/* ��ȡ�ڴ� */
NTSTATUS MyReadProcessMemory(HANDLE PID, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size)
{
	PEPROCESS SourceProcess;
	NTSTATUS Status = PsLookupProcessByProcessId(PID, &SourceProcess);
	if (Status != STATUS_SUCCESS) return Status;

	SIZE_T Result;
	PEPROCESS TargetProcess = PsGetCurrentProcess();
	Status = MmCopyVirtualMemory(SourceProcess, SourceAddr, TargetProcess, TargetAddr, Size, KernelMode, &Result);
	return Status;
}

/* д���ڴ� */
NTSTATUS MyWriteProcessMemory(HANDLE PID, PVOID SourceAddr, PVOID TargetAddr, SIZE_T Size)
{
	PEPROCESS SourceProcess;
	NTSTATUS Status = PsLookupProcessByProcessId(PID, &SourceProcess);
	if (Status != STATUS_SUCCESS) return Status;

	SIZE_T Result;
	PEPROCESS TargetProcess = PsGetCurrentProcess();
	Status = MmCopyVirtualMemory(TargetProcess, TargetAddr, SourceProcess, SourceAddr, Size, KernelMode, &Result);
	return Status;
}

/* ���� */
NTSTATUS CreateFunction(PDEVICE_OBJECT device, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

/* �ر� */
NTSTATUS CloseFunction(PDEVICE_OBJECT device, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

/* ���� */
NTSTATUS ControlFunction(PDEVICE_OBJECT device, PIRP irp)
{
	/* ��ȡIRPջλ�� */
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(irp);

	/* ��ȡ��Ӧ������ */
	ULONG Code = Stack->Parameters.DeviceIoControl.IoControlCode;

	/* ���ݽṹ */
	struct DataStruct Data;

	/* ����״̬ */
	NTSTATUS Status = STATUS_SUCCESS;

	switch (Code) //��ȡ�ڴ��ػ���д���ڴ���>
	{
	case IOCTL_READ://��ȡ�ڴ�
	{
		/* �������� */
		memcpy(&Data, irp->AssociatedIrp.SystemBuffer, sizeof(Data));

		/* ��ȡ��������� */
		PUCHAR Buffer = MmGetSystemAddressForMdl(irp->MdlAddress, NormalPagePriority);

		/* ��ȡ�ڴ� */
		if (Buffer && Data.Addr)
		{
			Status = MyReadProcessMemory(Data.PID, Data.Addr, Buffer, Data.Size);
			if (Status == STATUS_SUCCESS) KeFlushIoBuffers(irp->MdlAddress, TRUE, FALSE);
			else DebugMessage("��ȡ�������� : %d ", Status);
		}
		break;
	}
	case IOCTL_WRITE://д���ڴ�
	{
		/* �������� */
		memcpy(&Data, irp->AssociatedIrp.SystemBuffer, sizeof(Data));

		/* ��ȡ��������� */
		PUCHAR Buffer = MmGetSystemAddressForMdl(irp->MdlAddress, NormalPagePriority);

		/* д���ڴ� */
		if (Buffer && Data.Addr)
		{
			Status = MyWriteProcessMemory(Data.PID, Data.Addr, Buffer, Data.Size);
			if (Status == STATUS_SUCCESS) KeFlushIoBuffers(irp->MdlAddress, TRUE, FALSE);
			else DebugMessage("д�뷢������ : %d", Status);
		}
		break;
	}
	}

	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

/* ����ж�غ��� */
VOID DriverUnload(PDRIVER_OBJECT driver)
{
	/* ɾ���豸���ӷ��� */
	IoDeleteSymbolicLink(&g_SymbolicLink);

	/* ɾ���豸 */
	IoDeleteDevice(driver->DeviceObject);

	DebugMessage("[+] �豸ж�سɹ�");
}

/* ���������������ں��� */
NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING reg)
{
	/* �����豸 */
	IoCreateDevice(driver, 0, &g_DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &g_DeviceObject);

	/* �����豸���ӷ��� */
	IoCreateSymbolicLink(&g_SymbolicLink, &g_DeviceName);

	/* ������Ӧ������ */
	driver->MajorFunction[IRP_MJ_CREATE] = CreateFunction;
	driver->MajorFunction[IRP_MJ_CLOSE] = CloseFunction;
	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ControlFunction;

	/* ��������ж�غ��� */
	driver->DriverUnload = DriverUnload;

	/* ����Ϊֱ�ӷ�ʽ��д */
	g_DeviceObject->Flags |= DO_DIRECT_IO;

	/* ������DO_DEVICE_INITIALIZING��־ʱ��I/O���������ܾ����д򿪸��豸����������Լ�����豸�����ϸ��������豸���� */
	g_DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	DebugMessage("[+] �豸װ�سɹ�");
	return STATUS_SUCCESS;
}