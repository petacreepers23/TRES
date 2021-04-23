if(IsNearJump()) {
	if(IsRelativeJump()) 
		TemporaryEIP = EIP + Destination; //EIP is instruction following JMP instruction;
	else 
		TemporaryEIP == Destination;

	if(!IsWithinCodeSegmentLimits(TemporaryEIP)) 
		Exception(GP(0));

	if(OperandSize == 32) 
		EIP = TemporaryEIP;
	else 
		EIP = TemporaryEIP & 0xFFFF; //OperandSize == 16
	//END
}

if(IsFarJump() && (PE == 0 || (PE == 1 && VM == 1)) { //real-address or virtual-8086 mode
	TemporaryEIP = Destination.Offset; //Destination is ptr16:32 or [m16:32]
	if(!IsWithinCodeSegmentLimits(TemporaryEIP))
		Exception(GP(0));

	CS = Destination.SegmentSelector; //Destination ptr16:32 [m16:32]

	if(OperandSize == 32) 
		EIP = TemporaryEIP; //Destination ptr16:32 [m16:32]
	else /*OperandSize == 16*/ 
		EIP = TemporaryEIP & 0xFFFF; //clear upper 16 bits;
	//END
}

if(IsFarJump() && PE == 1 && VM == 0) { //Protected mode, not virtual-8086 mode
	SegmentsToCheck[] = {CS, DS, ES, FS, GS, SS};

	if(!CheckEffectiveAddresses(SegmentsToCheck) || TargetOperand.SegmentSelector == 0) 
			Exception(GP(0)); //effective address in the CS, DS, ES, FS, GS, or SS segment is illegal

	if(!IsWithinDescriptorTableLimits(SegmentSelector.Index)) 
		Exception(GP(NewSelector));

	SegmentDescriptorData = ReadSegmentDescriptorData(); //Read type and access rights of segment descriptor

	switch(SegmentDescriptorData.Type) {
		case TypeConformingCodeSegment:
			if(DPL > CPL) 
				Exception(GP(SegmentSelector));

			if(!IsPresent(Segment))
				Exception(NP(SegmentSelector));

			TemporaryEIP = Destination.Offset;

			if(OperandSize == 16) 
				TemporaryEIP = TemporaryEIP & 0xFFFF;

			if(!isWithinCodeSegmentLimits(TemporaryEIP)) 
				Exception(GP(0));

			CS = Destination.SegmentSelector; //segment descriptor information also loaded
			CS.RPL = CPL;
			EIP = TemporaryEIP;
			break;

		case TypeNonConformingCodeSegment:
			if(RPL > CPL || DPL != CPL) 
				Exception(GP(CodeSegmentSelector));

			if(!IsPresent(Segment)) 
				Exception(NP(SegmentSelector));

			if(!IsWithinCodeSegmentLimits(InstructionPointer)) 
				Exception(GP(0));

			TemporaryEIP = Destination.Offset;
			if(OperandSize == 16) 
				TemporaryEIP = TemporaryEIP & 0xFFFF;

			if(!IsWithinCodeSegmentLimits(TemporaryEIP)) 
				Exception(GP(0));

			CS = Destination.SegmentSelector; //segment descriptor information also loaded
			CS.RPL = CPL;
			EIP = TemporaryEIP;
			break;

		case TypeCallGate:
			if(CallGate.DPL < CPL || CallGate.DPL < CallGate.SegmentSelector.RPL) Exception(GP(CallGate.Selector));
			if(!IsPresent(CallGate)) Exception(NP(CallGate.Selector));
			if(CallGate.CodeSegmentSelector == 0) Exception(GP(0));
			CodeSegmentDescriptor = ReadCodeSegmentDescriptor();
			if(!IndicatesCodeSegment(CodeSegmentDescriptor) || (IsConforming(CodeSegmentDescriptor) && DPL > CPL) || (!IsConforming(CodeSegmentDescriptor) && DPL != CPL)) Exception(GP(CodeSegmentSelector));
			if(!IsPresent(CodeSegment)) Exception(NP(CodeSegmentSelector));
			if(!IsWithinCodeSegmentLimits(InstructionPointer)) Exception(GP(0));
			TemporaryEIP = Destination.Offset;
			if(GateSize == 16) TemporaryEIP = TemporaryEIP & 0xFFFF;
			if(!IsWithinCodeSegmentLimits(TemporaryEIP)) Exception(GP(0));
			CS = Destination.SegmentSelector; //segment descriptor information also loaded
			CS.RPL = CPL;
			EIP = TemporaryEIP;
			break;
		case TypeTaskGate:
			if(TaskGate.DPL < CPL || TaskGate.DPL < TaskGate.SegmentSelector.RPL) Exception(GP(TaskGate.Selector));
			if(!IsPresent(TaskGate)) Exception(NP(TaskGate.Selector));
			TSSSegmentSelector = ReadTSSSegmentSelector(TaskGateDescriptor); //Read the TSS segment selector in the task-gate descriptor;
			if(!IsGlobal(TSSSegmentSelector) /*TSS segment selector local/global bit is set to local*/ || !IsWithinGDTLimits(Index) || TSSIsBusy(TSSDescriptor)) Exception(GP(TSSSelector));
			if(!IsPresent(TSS)) Exception(NP(TSSSelector));
			SwitchTasks(TSS); //Switch tasks to TSS
			if(!IsWithinCodeSegmentLimit(EIP)) Exception(GP(0));
			break;
		case TypeTaskStateSegment:
			if(TSS.DPL < CPL || TSS.DPL < TSS.SegmentSelector.RPL || !TSSIsAvailable(TSSDescriptor) /*TSS descriptor indicates that TSS is not available*/) Exception(GP(TSSSelector));
			if(!IsPresent(TSS)) Exception(NP(TSSSelector));
			SwitchTasks(TSS); //Switch tasks to TSS
			if(!IsWithinCodeSegmentLimit(EIP)) Exception(GP(0));
			break;
		default:
			Exception(GP(SegmentSelector));
	}
}
else Exception(GP(SegmentSelector));
