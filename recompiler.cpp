    { case PPC_INST_ADD:
        println("\t{}.u64 = {}.u64 + {}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ADDC:
        println("\t{}.ca = ({}.u32 + {}.u32 < {}.u32);", xer(), r(insn.operands[1]), r(insn.operands[2]), r(insn.operands[1]));
        println("\t{}.u64 = {}.u64 + {}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ADDE:
        println("\t{}.u8 = ({}.u32 + {}.u32 < {}.u32) | ({}.u32 + {}.u32 + {}.ca < {}.ca);", temp(), r(insn.operands[1]), r(insn.operands[2]), r(insn.operands[1]), r(insn.operands[1]), r(insn.operands[2]), xer(), xer());
        println("\t{}.u64 = {}.u64 + {}.u64 + {}.ca;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]), xer());
        println("\t{}.ca = {}.u8;", xer(), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ADDI:
        print("\t{}.s64 = ", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.s64 + ", r(insn.operands[1]));
        println("{};", int32_t(insn.operands[2]));
        break;

    case PPC_INST_ADDIC:
        println("\t{}.ca = {}.u32 > {};", xer(), r(insn.operands[1]), ~insn.operands[2]);
        println("\t{}.s64 = {}.s64 + {};", r(insn.operands[0]), r(insn.operands[1]), int32_t(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ADDIS:
        print("\t{}.s64 = ", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.s64 + ", r(insn.operands[1]));
        println("{};", static_cast<int32_t>(insn.operands[2] << 16));
        break;

    case PPC_INST_ADDME:
        println("\t{}.u64 = {}.u64 + {}.ca - 1;", temp(), r(insn.operands[1]), xer());
        println("\t{}.ca = ({}.u64 > {}.u64) || ({}.u64 == {}.u64 && {}.ca);", xer(), 
            r(insn.operands[1]), temp(), r(insn.operands[1]), temp(), xer());
        println("\t{}.u64 = {}.u64;", r(insn.operands[0]), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", 
                cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ADDZE:
        println("\t{}.s64 = {}.s64 + {}.ca;", temp(), r(insn.operands[1]), xer());
        println("\t{}.ca = {}.u32 < {}.u32;", xer(), temp(), r(insn.operands[1]));
        println("\t{}.s64 = {}.s64;", r(insn.operands[0]), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_AND:
        println("\t{}.u64 = {}.u64 & {}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ANDC:
        println("\t{}.u64 = {}.u64 & ~{}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ANDI:
        println("\t{}.u64 = {}.u64 & {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ANDIS:
        println("\t{}.u64 = {}.u64 & {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2] << 16);
        println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ATTN:
        // undefined instruction
        break;

    case PPC_INST_B:
        if (insn.operands[0] < fn.base || insn.operands[0] >= fn.base + fn.size)
        {
            printFunctionCall(insn.operands[0]);
            println("\treturn;");
        }
        else
        {
            println("\tgoto loc_{:X};", insn.operands[0]);
        }
        break;

    case PPC_INST_BCTR:
        if (switchTable != config.switchTables.end())
        {
            println("\tswitch ({}.u64) {{", r(switchTable->second.r));

            for (size_t i = 0; i < switchTable->second.labels.size(); i++)
            {
                println("\tcase {}:", i);
                auto label = switchTable->second.labels[i];
                if (label < fn.base || label >= fn.base + fn.size)
                {
                    println("\t\t// ERROR: 0x{:X}", label);
                    fmt::println("ERROR: Switch case at {:X} is trying to jump outside function: {:X}", base, label);
                    println("\t\treturn;");
                }
                else
                {
                    println("\t\tgoto loc_{:X};", label);
                }
            }

            println("\tdefault:");
            println("\t\t__builtin_unreachable();");
            println("\t}}");

            switchTable = config.switchTables.end();
        }
        else
        {
            println("\tPPC_CALL_INDIRECT_FUNC({}.u32);", ctr());
            println("\treturn;");
        }
        break;

    case PPC_INST_BCTRL:
        if (!config.skipLr)
            println("\tctx.lr = 0x{:X};", base + 4);
        println("\tPPC_CALL_INDIRECT_FUNC({}.u32);", ctr());
        csrState = CSRState::Unknown; // the call could change it
        break;

    case PPC_INST_BDZ:
        println("\t--{}.u64;", ctr());
        println("\tif ({}.u32 == 0) goto loc_{:X};", ctr(), insn.operands[0]);
        break;

    case PPC_INST_BDZLR:
        println("\t--{}.u64;", ctr());
        println("\tif ({}.u32 == 0) return;", ctr(), insn.operands[0]);
        break;

    case PPC_INST_BDNZ:
        println("\t--{}.u64;", ctr());
        println("\tif ({}.u32 != 0) goto loc_{:X};", ctr(), insn.operands[0]);
        break;

    case PPC_INST_BDNZF:
        // NOTE: assuming eq here as a shortcut because all the instructions in the game do that
        println("\t--{}.u64;", ctr());
        println("\tif ({}.u32 != 0 && !{}.eq) goto loc_{:X};", ctr(), cr(insn.operands[0] / 4), insn.operands[1]);
        break;

    case PPC_INST_BDNZT:
        // NOTE(crack): Same note as BDNZF but true instead of false
        println("\t--{}.u64;", ctr());
        println("\tif ({}.u32 != 0 && {}.eq) goto loc_{:X};", ctr(), cr(insn.operands[0] / 4), insn.operands[1]);
        break;

    case PPC_INST_BEQ:
        printConditionalBranch(false, "eq");
        break;

    case PPC_INST_BEQLR:
        println("\tif ({}.eq) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_BGE:
        printConditionalBranch(true, "lt");
        break;

    case PPC_INST_BGELR:
        println("\tif (!{}.lt) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_BGT:
        printConditionalBranch(false, "gt");
        break;

    case PPC_INST_BGTLR:
        println("\tif ({}.gt) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_BL:
        if (!config.skipLr)
            println("\tctx.lr = 0x{:X};", base + 4);
        printFunctionCall(insn.operands[0]);
        csrState = CSRState::Unknown; // the call could change it
        break;

    case PPC_INST_BLE:
        printConditionalBranch(true, "gt");
        break;

    case PPC_INST_BLELR:
        println("\tif (!{}.gt) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_BLR:
        println("\treturn;");
        break;

    case PPC_INST_BLRL:
        println("__builtin_debugtrap();");
        break;

    case PPC_INST_BLT:
        printConditionalBranch(false, "lt");
        break;

    case PPC_INST_BLTLR:
        println("\tif ({}.lt) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_BNE:
        printConditionalBranch(true, "eq");
        break;

    case PPC_INST_BNECTR:
        println("\tif (!{}.eq) {{", cr(insn.operands[0]));
        println("\t\tPPC_CALL_INDIRECT_FUNC({}.u32);", ctr());
        println("\t\treturn;");
        println("\t}}");
        break;

    case PPC_INST_BNELR:
        println("\tif (!{}.eq) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_CCTPL:
        // no op
        break;

    case PPC_INST_CCTPM:
        // no op
        break;

    case PPC_INST_CLRLDI:
        println("\t{}.u64 = {}.u64 & 0x{:X};", r(insn.operands[0]), r(insn.operands[1]), (1ull << (64 - insn.operands[2])) - 1);
        break;

    case PPC_INST_CLRLWI:
        println("\t{}.u64 = {}.u32 & 0x{:X};", r(insn.operands[0]), r(insn.operands[1]), (1ull << (32 - insn.operands[2])) - 1);
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_CMPD:
        println("\t{}.compare<int64_t>({}.s64, {}.s64, {});", cr(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]), xer());
        break;

    case PPC_INST_CMPDI:
        println("\t{}.compare<int64_t>({}.s64, {}, {});", cr(insn.operands[0]), r(insn.operands[1]), int32_t(insn.operands[2]), xer());
        break;

    case PPC_INST_CMPLD:
        println("\t{}.compare<uint64_t>({}.u64, {}.u64, {});", cr(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]), xer());
        break;

    case PPC_INST_CMPLDI:
        println("\t{}.compare<uint64_t>({}.u64, {}, {});", cr(insn.operands[0]), r(insn.operands[1]), insn.operands[2], xer());
        break;

    case PPC_INST_CMPLW:
        println("\t{}.compare<uint32_t>({}.u32, {}.u32, {});", cr(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]), xer());
        break;

    case PPC_INST_CMPLWI:
        println("\t{}.compare<uint32_t>({}.u32, {}, {});", cr(insn.operands[0]), r(insn.operands[1]), insn.operands[2], xer());
        break;

    case PPC_INST_CMPW:
        println("\t{}.compare<int32_t>({}.s32, {}.s32, {});", cr(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]), xer());
        break;

    case PPC_INST_CMPWI:
        println("\t{}.compare<int32_t>({}.s32, {}, {});", cr(insn.operands[0]), r(insn.operands[1]), int32_t(insn.operands[2]), xer());
        break;

    case PPC_INST_CNTLZD:
        println("\t{0}.u64 = {1}.u64 == 0 ? 64 : __builtin_clzll({1}.u64);", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_CNTLZW:
        println("\t{0}.u64 = {1}.u32 == 0 ? 32 : __builtin_clz({1}.u32);", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_DB16CYC:
        // no op
        break;

    case PPC_INST_DCBF:
        // no op
        break;

    case PPC_INST_DCBT:
        // no op
        break;

    case PPC_INST_DCBST:
    case PPC_INST_DCBTST:
        // no op
        break;

    case PPC_INST_DCBZ:
        print("\tmemset(base + ((");
        if (insn.operands[0] != 0)
            print("{}.u32 + ", r(insn.operands[0]));
        println("{}.u32) & ~31), 0, 32);", r(insn.operands[1]));
        break;

    case PPC_INST_DCBZL:
        print("\tmemset(base + ((");
        if (insn.operands[0] != 0)
            print("{}.u32 + ", r(insn.operands[0]));
        println("{}.u32) & ~127), 0, 128);", r(insn.operands[1]));
        break;

    case PPC_INST_DIVD:
        println("\t{}.s64 = {}.s64 / {}.s64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_DIVDU:
        println("\t{}.u64 = {}.u64 / {}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_DIVW:
        println("\t{}.s32 = {}.s32 / {}.s32;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_DIVWU:
        println("\t{}.u32 = {}.u32 / {}.u32;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_EIEIO:
        // no op
        break;

    case PPC_INST_EQV:
        // rA = ~(rS XOR rB)
        println("\t{}.u64 = ~({}.u64 ^ {}.u64);", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_EXTSB:
        println("\t{}.s64 = {}.s8;", r(insn.operands[0]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_EXTSH:
        println("\t{}.s64 = {}.s16;", r(insn.operands[0]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_EXTSW:
        println("\t{}.s64 = {}.s32;", r(insn.operands[0]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_FABS:
        printSetFlushMode(false);
        println("\t{}.u64 = {}.u64 & ~0x8000000000000000;", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FADD:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 + {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FADDS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64 + {}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FCFID:
        printSetFlushMode(false);
        println("\t{}.f64 = double({}.s64);", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FCMPU:
        printSetFlushMode(false);
        println("\t{}.compare({}.f64, {}.f64);", cr(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FCTID:
        printSetFlushMode(false);
        println("\t{}.s64 = ({}.f64 > double(LLONG_MAX)) ? LLONG_MAX : _mm_cvtsd_si64(_mm_load_sd(&{}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[1]));
        break;

    case PPC_INST_FCTIDZ:
        printSetFlushMode(false);
        println("\t{}.s64 = ({}.f64 > double(LLONG_MAX)) ? LLONG_MAX : _mm_cvttsd_si64(_mm_load_sd(&{}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[1]));
        break;

    case PPC_INST_FCTIWZ:
        printSetFlushMode(false);
        println("\t{}.s64 = ({}.f64 > double(INT_MAX)) ? INT_MAX : _mm_cvttsd_si32(_mm_load_sd(&{}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[1]));
        break;

    case PPC_INST_FDIV:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 / {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FDIVS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64 / {}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FMADD:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 * {}.f64 + {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FMADDS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64 * {}.f64 + {}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FMR:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64;", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FMSUB:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 * {}.f64 - {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FMSUBS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64 * {}.f64 - {}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FMUL:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 * {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FMULS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64 * {}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FNABS:
        printSetFlushMode(false);
        println("\t{}.u64 = {}.u64 | 0x8000000000000000;", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FNEG:
        printSetFlushMode(false);
        println("\t{}.u64 = {}.u64 ^ 0x8000000000000000;", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FNMADDS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float(-({}.f64 * {}.f64 + {}.f64)));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FNMSUB:
        printSetFlushMode(false);
        println("\t{}.f64 = -({}.f64 * {}.f64 - {}.f64);", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FNMSUBS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float(-({}.f64 * {}.f64 - {}.f64)));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FRES:
        printSetFlushMode(false);
        println("\t{}.f64 = float(1.0 / {}.f64);", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FRSP:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64));", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FSEL:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 >= 0.0 ? {}.f64 : {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        break;

    case PPC_INST_FRSQRTE:
        // TODO(crack): I sure hope the c++ optimizer can optimize this. Fixme with some simd magic later
        printSetFlushMode(false);
        println("\t{}.f64 = double(1.0f / sqrtf(float({}.f64)));", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FSQRT:
        printSetFlushMode(false);
        println("\t{}.f64 = sqrt({}.f64);", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FSQRTS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float(sqrt({}.f64)));", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_FSUB:
        printSetFlushMode(false);
        println("\t{}.f64 = {}.f64 - {}.f64;", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_FSUBS:
        printSetFlushMode(false);
        println("\t{}.f64 = double(float({}.f64 - {}.f64));", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]));
        break;

    case PPC_INST_LBZ:
        print("\t{}.u64 = PPC_LOAD_U8(", r(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{});", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LBZU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\t{}.u64 = PPC_LOAD_U8({});", r(insn.operands[0]), ea());
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_LBZX:
        print("\t{}.u64 = PPC_LOAD_U8(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        break;

    case PPC_INST_LD:
        print("\t{}.u64 = PPC_LOAD_U64(", r(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{});", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LDARX:
        print("\t{}.u64 = *(uint64_t*)(base + ", reserved());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        println("\t{}.u64 = __builtin_bswap64({}.u64);", r(insn.operands[0]), reserved());
        break;

    case PPC_INST_LDU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\t{}.u64 = PPC_LOAD_U64({});", r(insn.operands[0]), ea());
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_LDX:
        print("\t{}.u64 = PPC_LOAD_U64(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        break;

    case PPC_INST_LFD:
        printSetFlushMode(false);
        print("\t{}.u64 = PPC_LOAD_U64(", f(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{});", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LFDX:
        printSetFlushMode(false);
        print("\t{}.u64 = PPC_LOAD_U64(", f(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        break;

    case PPC_INST_LFS:
        printSetFlushMode(false);
        print("\t{}.u32 = PPC_LOAD_U32(", temp());
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{});", int32_t(insn.operands[1]));
        println("\t{}.f64 = double({}.f32);", f(insn.operands[0]), temp());
        break;

    case PPC_INST_LFSX:
        printSetFlushMode(false);
        print("\t{}.u32 = PPC_LOAD_U32(", temp());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        println("\t{}.f64 = double({}.f32);", f(insn.operands[0]), temp());
        break;

    case PPC_INST_LHA:
        print("\t{}.s64 = int16_t(PPC_LOAD_U16(", r(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}));", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LHAX:
        print("\t{}.s64 = int16_t(PPC_LOAD_U16(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32));", r(insn.operands[2]));
        break;

    case PPC_INST_LHZ:
        print("\t{}.u64 = PPC_LOAD_U16(", r(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{});", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LHZX:
        print("\t{}.u64 = PPC_LOAD_U16(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        break;

    case PPC_INST_LI:
        println("\t{}.s64 = {};", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_LIS:
        println("\t{}.s64 = {};", r(insn.operands[0]), int32_t(insn.operands[1] << 16));
        break;

    case PPC_INST_LVEBX:
    case PPC_INST_LVEHX:
    case PPC_INST_LVEWX:
    case PPC_INST_LVEWX128:
    case PPC_INST_LVX:
    case PPC_INST_LVX128:
        // NOTE: for endian swapping, we reverse the whole vector instead of individual elements.
        // this is accounted for in every instruction (eg. dp3 sums yzw instead of xyz)
        print("\t_mm_store_si128((__m128i*){}.u8, _mm_shuffle_epi8(_mm_load_si128((__m128i*)(base + ((", v(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32) & ~0xF))), _mm_load_si128((__m128i*)VectorMaskL)));", r(insn.operands[2]));
        break;

    case PPC_INST_LVLX:
    case PPC_INST_LVLX128:
        print("\t{}.u32 = ", temp());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32;", r(insn.operands[2]));
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_shuffle_epi8(_mm_load_si128((__m128i*)(base + ({}.u32 & ~0xF))), _mm_load_si128((__m128i*)&VectorMaskL[({}.u32 & 0xF) * 16])));", v(insn.operands[0]), temp(), temp());
        break;

    case PPC_INST_LVRX:
    case PPC_INST_LVRX128:
        print("\t{}.u32 = ", temp());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32;", r(insn.operands[2]));
        println("\t_mm_store_si128((__m128i*){}.u8, {}.u32 & 0xF ? _mm_shuffle_epi8(_mm_load_si128((__m128i*)(base + ({}.u32 & ~0xF))), _mm_load_si128((__m128i*)&VectorMaskR[({}.u32 & 0xF) * 16])) : _mm_setzero_si128());", v(insn.operands[0]), temp(), temp(), temp());
        break;

    case PPC_INST_LVSL:
        print("\t{}.u32 = ", temp());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32;", r(insn.operands[2]));
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_load_si128((__m128i*)&VectorShiftTableL[({}.u32 & 0xF) * 16]));", v(insn.operands[0]), temp());
        break;

    case PPC_INST_LVSR:
        print("\t{}.u32 = ", temp());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32;", r(insn.operands[2]));
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_load_si128((__m128i*)&VectorShiftTableR[({}.u32 & 0xF) * 16]));", v(insn.operands[0]), temp());
        break;

    case PPC_INST_LWA:
        print("\t{}.s64 = int32_t(PPC_LOAD_U32(", r(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}));", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LWARX:
        print("\t{}.u32 = *(uint32_t*)(base + ", reserved());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        println("\t{}.u64 = __builtin_bswap32({}.u32);", r(insn.operands[0]), reserved());
        break;

    case PPC_INST_LWAX:
        print("\t{}.s64 = int32_t(PPC_LOAD_U32(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32));", r(insn.operands[2]));
        break;

    case PPC_INST_LWBRX:
        print("\t{}.u64 = __builtin_bswap32(PPC_LOAD_U32(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32));", r(insn.operands[2]));
        break;

    case PPC_INST_LWSYNC:
        // no op
        break;

    case PPC_INST_LWZ:
        print("\t{}.u64 = PPC_LOAD_U32(", r(insn.operands[0]));
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{});", int32_t(insn.operands[1]));
        break;

    case PPC_INST_LWZU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\t{}.u64 = PPC_LOAD_U32({});", r(insn.operands[0]), ea());
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_LWZX:
        print("\t{}.u64 = PPC_LOAD_U32(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32);", r(insn.operands[2]));
        break;

    case PPC_INST_MFCR:
        for (size_t i = 0; i < 32; i++)
        {
            constexpr std::string_view fields[] = { "lt", "gt", "eq", "so" };
            println("\t{}.u64 {}= {}.{} ? 0x{:X} : 0;", r(insn.operands[0]), i == 0 ? "" : "|", cr(i / 4), fields[i % 4], 1u << (31 - i));
        }
        break;

    case PPC_INST_MFFS:
         println("\t{}.u64 = ctx.fpscr.loadFromHost();", f(insn.operands[0]));
        break;

    case PPC_INST_MFLR:
        if (!config.skipLr)
            println("\t{}.u64 = ctx.lr;", r(insn.operands[0]));
        break;

    case PPC_INST_MFMSR:
        if (!config.skipMsr)
            println("\t{}.u64 = ctx.msr;", r(insn.operands[0]));
        break;

    case PPC_INST_MFOCRF:
        // TODO: don't hardcode to cr6
        println("\t{}.u64 = ({}.lt << 7) | ({}.gt << 6) | ({}.eq << 5) | ({}.so << 4);", r(insn.operands[0]), cr(6), cr(6), cr(6), cr(6));
        break;

    case PPC_INST_MFTB:
        println("\t{}.u64 = __rdtsc();", r(insn.operands[0]));
        break;

    case PPC_INST_MR:
        println("\t{}.u64 = {}.u64;", r(insn.operands[0]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_MTCR:
        for (size_t i = 0; i < 32; i++)
        {
            constexpr std::string_view fields[] = { "lt", "gt", "eq", "so" };
            println("\t{}.{} = ({}.u32 & 0x{:X}) != 0;", cr(i / 4), fields[i % 4], r(insn.operands[0]), 1u << (31 - i));
        }
        break;

    case PPC_INST_MTCTR:
        println("\t{}.u64 = {}.u64;", ctr(), r(insn.operands[0]));
        break;

    case PPC_INST_MTFSF:
        println("\tctx.fpscr.storeFromGuest({}.u32);", f(insn.operands[1]));
        break;

    case PPC_INST_MTLR:
        if (!config.skipLr)
            println("\tctx.lr = {}.u64;", r(insn.operands[0]));
        break;

    case PPC_INST_MTMSRD:
        if (!config.skipMsr)
            println("\tctx.msr = ({}.u32 & 0x8020) | (ctx.msr & ~0x8020);", r(insn.operands[0]));
        break;

    case PPC_INST_MTXER:
        println("\t{}.so = ({}.u64 & 0x80000000) != 0;", xer(), r(insn.operands[0]));
        println("\t{}.ov = ({}.u64 & 0x40000000) != 0;", xer(), r(insn.operands[0]));
        println("\t{}.ca = ({}.u64 & 0x20000000) != 0;", xer(), r(insn.operands[0]));
        break;

    case PPC_INST_MULHW:
        println("\t{}.s64 = (int64_t({}.s32) * int64_t({}.s32)) >> 32;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_MULHWU:
        println("\t{}.u64 = (uint64_t({}.u32) * uint64_t({}.u32)) >> 32;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_MULLD:
        println("\t{}.s64 = {}.s64 * {}.s64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_MULLI:
        println("\t{}.s64 = {}.s64 * {};", r(insn.operands[0]), r(insn.operands[1]), int32_t(insn.operands[2]));
        break;

    case PPC_INST_MULLW:
        println("\t{}.s64 = int64_t({}.s32) * int64_t({}.s32);", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_MULHD:
        println("\t{}.s64 = __mulh({}.s64, {}.s64);",
            r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", 
                cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_MULHDU:
        println("\t{}.u64 = __mulhu({}.u64, {}.u64);", 
            r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", 
                cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_NAND:
        println("\t{}.u64 = ~({}.u64 & {}.u64);", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_NEG:
        println("\t{}.s64 = -{}.s64;", r(insn.operands[0]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_NOP:
        // no op
        break;

    case PPC_INST_NOR:
        println("\t{}.u64 = ~({}.u64 | {}.u64);", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_NOT:
        println("\t{}.u64 = ~{}.u64;", r(insn.operands[0]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_OR:
        println("\t{}.u64 = {}.u64 | {}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ORC:
        println("\t{}.u64 = {}.u64 | ~{}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_ORI:
        println("\t{}.u64 = {}.u64 | {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        break;

    case PPC_INST_ORIS:
        println("\t{}.u64 = {}.u64 | {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2] << 16);
        break;

    case PPC_INST_RLDICL:
        println("\t{}.u64 = __builtin_rotateleft64({}.u64, {}) & 0x{:X};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2], ComputeMask(insn.operands[3], 63));
        break;

    case PPC_INST_RLDICR:
        println("\t{}.u64 = __builtin_rotateleft64({}.u64, {}) & 0x{:X};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2], ComputeMask(0, insn.operands[3]));
        break;

    case PPC_INST_RLDIMI:
    {
        const uint64_t mask = ComputeMask(insn.operands[3], ~insn.operands[2]);
        println("\t{}.u64 = (__builtin_rotateleft64({}.u64, {}) & 0x{:X}) | ({}.u64 & 0x{:X});", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2], mask, r(insn.operands[0]), ~mask);
        break;

    case PPC_INST_RLWIMI:
    {
        const uint64_t mask = ComputeMask(insn.operands[3] + 32, insn.operands[4] + 32);
        println("\t{}.u64 = (__builtin_rotateleft32({}.u32, {}) & 0x{:X}) | ({}.u64 & 0x{:X});", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2], mask, r(insn.operands[0]), ~mask);
        break;

    case PPC_INST_RLWINM:
        println("\t{}.u64 = __builtin_rotateleft64({}.u32 | ({}.u64 << 32), {}) & 0x{:X};", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[1]), insn.operands[2], ComputeMask(insn.operands[3] + 32, insn.operands[4] + 32));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_RLWNM:
        println("\t{}.u64 = __builtin_rotateleft64({}.u32 | ({}.u64 << 32), {}.u8 & 0x1F) & 0x{:X};", 
            r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[1]), 
            r(insn.operands[2]), ComputeMask(insn.operands[3] + 32, insn.operands[4] + 32));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_ROTLDI:
        println("\t{}.u64 = __builtin_rotateleft64({}.u64, {});", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        break;

    case PPC_INST_ROTLW:
        println("\t{}.u64 = __builtin_rotateleft32({}.u32, {}.u8 & 0x1F);", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_ROTLWI:
        println("\t{}.u64 = __builtin_rotateleft32({}.u32, {});", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SLD:
        println("\t{}.u64 = {}.u8 & 0x40 ? 0 : ({}.u64 << ({}.u8 & 0x7F));", r(insn.operands[0]), r(insn.operands[2]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_SLW:
        println("\t{}.u64 = {}.u8 & 0x20 ? 0 : ({}.u32 << ({}.u8 & 0x3F));", r(insn.operands[0]), r(insn.operands[2]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SRAD:
        println("\t{}.u64 = {}.u64 & 0x7F;", temp(), r(insn.operands[2]));
        println("\tif ({}.u64 > 0x3F) {}.u64 = 0x3F;", temp(), temp());
        println("\t{}.ca = ({}.s64 < 0) & ((({}.s64 >> {}.u64) << {}.u64) != {}.s64);", xer(), r(insn.operands[1]), r(insn.operands[1]), temp(), temp(), r(insn.operands[1]));
        println("\t{}.s64 = {}.s64 >> {}.u64;", r(insn.operands[0]), r(insn.operands[1]), temp());
        break;

    case PPC_INST_SRADI:
        if (insn.operands[2] != 0)
        {
            println("\t{}.ca = ({}.s64 < 0) & (({}.u64 & 0x{:X}) != 0);", xer(), r(insn.operands[1]), r(insn.operands[1]), ComputeMask(64 - insn.operands[2], 63));
            println("\t{}.s64 = {}.s64 >> {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        }
        else
        {
            println("\t{}.ca = 0;", xer());
            println("\t{}.s64 = {}.s64;", r(insn.operands[0]), r(insn.operands[1]));
        }
        break;

    case PPC_INST_SRAW:
        println("\t{}.u32 = {}.u32 & 0x3F;", temp(), r(insn.operands[2]));
        println("\tif ({}.u32 > 0x1F) {}.u32 = 0x1F;", temp(), temp());
        println("\t{}.ca = ({}.s32 < 0) & ((({}.s32 >> {}.u32) << {}.u32) != {}.s32);", xer(), r(insn.operands[1]), r(insn.operands[1]), temp(), temp(), r(insn.operands[1]));
        println("\t{}.s64 = {}.s32 >> {}.u32;", r(insn.operands[0]), r(insn.operands[1]), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SRAWI:
        if (insn.operands[2] != 0)
        {
            println("\t{}.ca = ({}.s32 < 0) & (({}.u32 & 0x{:X}) != 0);", xer(), r(insn.operands[1]), r(insn.operands[1]), ComputeMask(64 - insn.operands[2], 63));
            println("\t{}.s64 = {}.s32 >> {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        }
        else
        {
            println("\t{}.ca = 0;", xer());
            println("\t{}.s64 = {}.s32;", r(insn.operands[0]), r(insn.operands[1]));
        }
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SRD:
        println("\t{}.u64 = {}.u8 & 0x40 ? 0 : ({}.u64 >> ({}.u8 & 0x7F));", r(insn.operands[0]), r(insn.operands[2]), r(insn.operands[1]), r(insn.operands[2]));
        break;

    case PPC_INST_SRW:
        println("\t{}.u64 = {}.u8 & 0x20 ? 0 : ({}.u32 >> ({}.u8 & 0x3F));", r(insn.operands[0]), r(insn.operands[2]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_STB:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U8(" : "\tPPC_STORE_U8(");
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}, {}.u8);", int32_t(insn.operands[1]), r(insn.operands[0]));
        break;

    case PPC_INST_STBU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\tPPC_STORE_U8({}, {}.u8);", ea(), r(insn.operands[0]));
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_STBX:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U8(" : "\tPPC_STORE_U8(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u8);", r(insn.operands[2]), r(insn.operands[0]));
        break;

    case PPC_INST_STD:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U64(" : "\tPPC_STORE_U64(");
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}, {}.u64);", int32_t(insn.operands[1]), r(insn.operands[0]));
        break;

    case PPC_INST_STDCX:
        println("\t{}.lt = 0;", cr(0));
        println("\t{}.gt = 0;", cr(0));
        print("\t{}.eq = __sync_bool_compare_and_swap(reinterpret_cast<uint64_t*>(base + ", cr(0));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32), {}.s64, __builtin_bswap64({}.s64));", r(insn.operands[2]), reserved(), r(insn.operands[0]));
        println("\t{}.so = {}.so;", cr(0), xer());
        break;

    case PPC_INST_STDU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\tPPC_STORE_U64({}, {}.u64);", ea(), r(insn.operands[0]));
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_STDX:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U64(" : "\tPPC_STORE_U64(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u64);", r(insn.operands[2]), r(insn.operands[0]));
        break;

    case PPC_INST_STFD:
        printSetFlushMode(false);
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U64(" : "\tPPC_STORE_U64(");
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}, {}.u64);", int32_t(insn.operands[1]), f(insn.operands[0]));
        break;

    case PPC_INST_STFDX:
        printSetFlushMode(false);
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U64(" : "\tPPC_STORE_U64(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u64);", r(insn.operands[2]), f(insn.operands[0]));
        break;

    case PPC_INST_STFIWX:
        printSetFlushMode(false);
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U32(" : "\tPPC_STORE_U32(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u32);", r(insn.operands[2]), f(insn.operands[0]));
        break;

    case PPC_INST_STFS:
        printSetFlushMode(false);
        println("\t{}.f32 = float({}.f64);", temp(), f(insn.operands[0]));
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U32(" : "\tPPC_STORE_U32(");
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}, {}.u32);", int32_t(insn.operands[1]), temp());
        break;

    case PPC_INST_STFSX:
        printSetFlushMode(false);
        println("\t{}.f32 = float({}.f64);", temp(), f(insn.operands[0]));
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U32(" : "\tPPC_STORE_U32(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u32);", r(insn.operands[2]), temp());
        break;

    case PPC_INST_STH:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U16(" : "\tPPC_STORE_U16(");
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}, {}.u16);", int32_t(insn.operands[1]), r(insn.operands[0]));
        break;

    case PPC_INST_STHBRX:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U16(" : "\tPPC_STORE_U16(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, __builtin_bswap16({}.u16));", r(insn.operands[2]), r(insn.operands[0]));
        break;

    case PPC_INST_STHX:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U16(" : "\tPPC_STORE_U16(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u16);", r(insn.operands[2]), r(insn.operands[0]));
        break;

    case PPC_INST_STVEHX:
        // TODO: vectorize
        // NOTE: accounting for the full vector reversal here
        print("\t{} = (", ea());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32) & ~0x1;", r(insn.operands[2]));
        println("\tPPC_STORE_U16(ea, {}.u16[7 - (({} & 0xF) >> 1)]);", v(insn.operands[0]), ea());
        break;

    case PPC_INST_STVEWX:
    case PPC_INST_STVEWX128:
        // TODO: vectorize
        // NOTE: accounting for the full vector reversal here
        print("\t{} = (", ea());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32) & ~0x3;", r(insn.operands[2]));
        println("\tPPC_STORE_U32(ea, {}.u32[3 - (({} & 0xF) >> 2)]);", v(insn.operands[0]), ea());
        break;

    case PPC_INST_STVLX:
    case PPC_INST_STVLX128:
        // TODO: vectorize
        // NOTE: accounting for the full vector reversal here
        print("\t{} = ", ea());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32;", r(insn.operands[2]));

        println("\tfor (size_t i = 0; i < (16 - ({} & 0xF)); i++)", ea());
        println("\t\tPPC_STORE_U8({} + i, {}.u8[15 - i]);", ea(), v(insn.operands[0]));
        break;

    case PPC_INST_STVRX:
    case PPC_INST_STVRX128:
        // TODO: vectorize
        // NOTE: accounting for the full vector reversal here
        print("\t{} = ", ea());
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32;", r(insn.operands[2]));

        println("\tfor (size_t i = 0; i < ({} & 0xF); i++)", ea());
        println("\t\tPPC_STORE_U8({} - i - 1, {}.u8[i]);", ea(), v(insn.operands[0]));
        break;

    case PPC_INST_STVX:
    case PPC_INST_STVX128:
        print("\t_mm_store_si128((__m128i*)(base + ((");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32) & ~0xF)), _mm_shuffle_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*)VectorMaskL)));", r(insn.operands[2]), v(insn.operands[0]));
        break;

    case PPC_INST_STW:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U32(" : "\tPPC_STORE_U32(");
        if (insn.operands[2] != 0)
            print("{}.u32 + ", r(insn.operands[2]));
        println("{}, {}.u32);", int32_t(insn.operands[1]), r(insn.operands[0]));
        break;

    case PPC_INST_STWBRX:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U32(" : "\tPPC_STORE_U32(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, __builtin_bswap32({}.u32));", r(insn.operands[2]), r(insn.operands[0]));
        break;

    case PPC_INST_STWCX:
        println("\t{}.lt = 0;", cr(0));
        println("\t{}.gt = 0;", cr(0));
        print("\t{}.eq = __sync_bool_compare_and_swap(reinterpret_cast<uint32_t*>(base + ", cr(0));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32), {}.s32, __builtin_bswap32({}.s32));", r(insn.operands[2]), reserved(), r(insn.operands[0]));
        println("\t{}.so = {}.so;", cr(0), xer());
        break;

    case PPC_INST_STWU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\tPPC_STORE_U32({}, {}.u32);", ea(), r(insn.operands[0]));
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_STWUX:
        println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
        println("\tPPC_STORE_U32({}, {}.u32);", ea(), r(insn.operands[0]));
        println("\t{}.u32 = {};", r(insn.operands[1]), ea());
        break;

    case PPC_INST_STWX:
        print("{}", mmioStore() ? "\tPPC_MM_STORE_U32(" : "\tPPC_STORE_U32(");
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32, {}.u32);", r(insn.operands[2]), r(insn.operands[0]));
        break;

    case PPC_INST_SUBF:
        println("\t{}.s64 = {}.s64 - {}.s64;", r(insn.operands[0]), r(insn.operands[2]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SUBFC:
        println("\t{}.ca = {}.u32 >= {}.u32;", xer(), r(insn.operands[2]), r(insn.operands[1]));
        println("\t{}.s64 = {}.s64 - {}.s64;", r(insn.operands[0]), r(insn.operands[2]), r(insn.operands[1]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SUBFE:
        println("\t{}.u8 = (~{}.u32 + {}.u32 < ~{}.u32) | (~{}.u32 + {}.u32 + {}.ca < {}.ca);", temp(), r(insn.operands[1]), r(insn.operands[2]), r(insn.operands[1]), r(insn.operands[1]), r(insn.operands[2]), xer(), xer());
        println("\t{}.u64 = ~{}.u64 + {}.u64 + {}.ca;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]), xer());
        println("\t{}.ca = {}.u8;", xer(), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SUBFIC:
        println("\t{}.ca = {}.u32 <= {};", xer(), r(insn.operands[1]), insn.operands[2]);
        println("\t{}.s64 = {} - {}.s64;", r(insn.operands[0]), int32_t(insn.operands[2]), r(insn.operands[1]));
        break;

    case PPC_INST_SUBFME:
        println("\t{}.u64 = ~{}.u64 + {}.ca - 1;", temp(), r(insn.operands[1]), xer());
        println("\t{}.ca = ({}.u64 < ~{}.u64) || ({}.u64 == ~{}.u64 && {}.ca);", xer(), 
            temp(), r(insn.operands[1]), temp(), r(insn.operands[1]), xer());
        println("\t{}.u64 = {}.u64;", r(insn.operands[0]), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", 
                cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SUBFZE:
        println("\t{}.u64 = ~{}.u64 + {}.ca;", temp(), r(insn.operands[1]), xer());
        println("\t{}.ca = {}.u64 < {}.ca;", xer(), temp(), xer());
        println("\t{}.u64 = {}.u64;", r(insn.operands[0]), temp());
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_SYNC:
        // no op
        break;

    case PPC_INST_TDEQ:
        println("\tif ({}.u64 == {}.u64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDEQI:
        println("\tif ({}.u64 == {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TDGE:
        println("\tif ({}.s64 >= {}.s64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDGEI:
        println("\tif ({}.s64 >= {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TDGT:
        println("\tif ({}.s64 > {}.s64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDGTI:
        println("\tif ({}.s64 > {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TDLE:
        println("\tif ({}.s64 <= {}.s64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDLEI:
        println("\tif ({}.s64 <= {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TDLGE:
        println("\tif ({}.u64 >= {}.u64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDLGEI:
        println("\tif ({}.u64 >= {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TDLGT:
        println("\tif ({}.u64 > {}.u64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDLGTI:
        println("\tif ({}.u64 > {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TDLLE:
        println("\tif ({}.u64 <= {}.u64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDLLEI:
        println("\tif ({}.u64 <= {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TDLLT:
        println("\tif ({}.u64 < {}.u64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDLLTI:
        println("\tif ({}.u64 < {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TDLT:
        println("\tif ({}.s64 < {}.s64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDLTI:
        println("\tif ({}.s64 < {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TDNE:
        println("\tif ({}.u64 != {}.u64) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TDNEI:
        println("\tif ({}.u64 != {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TWI:
    {
        // TO field specifies trap conditions:
        // Bit 0 (16): Less than (signed)
        // Bit 1 (8): Greater than (signed)  
        // Bit 2 (4): Equal
        // Bit 3 (2): Less than (unsigned)
        // Bit 4 (1): Greater than (unsigned)
        
        bool first = true;
        print("\tif (");
        
        if (insn.operands[0] & 16) {
            print("{}.s32 < {}", r(insn.operands[1]), int32_t(insn.operands[2]));
            first = false;
        }
        
        if (insn.operands[0] & 8) {
            if (!first) print(" || ");
            print("{}.s32 > {}", r(insn.operands[1]), int32_t(insn.operands[2]));
            first = false;
        }
        
        if (insn.operands[0] & 4) {
            if (!first) print(" || ");
            print("{}.u32 == {}", r(insn.operands[1]), insn.operands[2]);
            first = false;
        }
        
        if (insn.operands[0] & 2) {
            if (!first) print(" || ");
            print("{}.u32 < {}", r(insn.operands[1]), insn.operands[2]);
            first = false;
        }
        
        if (insn.operands[0] & 1) {
            if (!first) print(" || ");
            print("{}.u32 > {}", r(insn.operands[1]), insn.operands[2]);
            first = false;
        }
        
        if (first) {
            // TO = 0 means never trap
            println("false) __builtin_debugtrap();");
        } else {
            println(") __builtin_debugtrap();");
        }
    }
    break;

    case PPC_INST_TWEQ:
        println("\tif ({}.u32 == {}.u32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWEQI:
        println("\tif ({}.u32 == {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TWGE:
        println("\tif ({}.s32 >= {}.s32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWGEI:
        println("\tif ({}.s32 >= {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TWGT:
        println("\tif ({}.s32 > {}.s32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWGTI:
        println("\tif ({}.s32 > {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TWLE:
        println("\tif ({}.s32 <= {}.s32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWLEI:
        println("\tif ({}.s32 <= {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TWLGE:
        println("\tif ({}.u32 >= {}.u32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWLGEI:
        println("\tif ({}.u32 >= {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TWLGT:
        println("\tif ({}.u32 > {}.u32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWLGTI:
        println("\tif ({}.u32 > {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TWLLE:
        println("\tif ({}.u32 <= {}.u32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWLLEI:
        println("\tif ({}.u32 <= {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TWLLT:
        println("\tif ({}.u32 < {}.u32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWLLTI:
        println("\tif ({}.u32 < {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_TWLT:
        println("\tif ({}.s32 < {}.s32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWLTI:
        println("\tif ({}.s32 < {}) __builtin_debugtrap();", r(insn.operands[0]), int32_t(insn.operands[1]));
        break;

    case PPC_INST_TWNE:
        println("\tif ({}.u32 != {}.u32) __builtin_debugtrap();", r(insn.operands[0]), r(insn.operands[1]));
        break;

    case PPC_INST_TWNEI:
        println("\tif ({}.u32 != {}) __builtin_debugtrap();", r(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_VADDFP:
    case PPC_INST_VADDFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_add_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VADDSHS:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_adds_epi16(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VADDSWS:
        // TODO(crack): vectorize - SSE doesn't have _mm_adds_epi32
        for (size_t i = 0; i < 4; i++)
        {
            println("\t{}.s64 = int64_t({}.s32[{}]) + int64_t({}.s32[{}]);", temp(), v(insn.operands[1]), i, v(insn.operands[2]), i);
            println("\t{}.s32[{}] = {}.s64 > INT_MAX ? INT_MAX : {}.s64 < INT_MIN ? INT_MIN : {}.s64;",
                v(insn.operands[0]), i, temp(), temp(), temp());
        }
        break;

    case PPC_INST_VADDUBM:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_add_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VADDUBS:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_adds_epu8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VADDUHM:
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_add_epi16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VADDUWM:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_add_epi32(_mm_load_si128((__m128i*){}.u32), _mm_load_si128((__m128i*){}.u32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VADDUWS:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_adds_epu32(_mm_load_si128((__m128i*){}.u32), _mm_load_si128((__m128i*){}.u32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VAND:
    case PPC_INST_VAND128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_and_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VANDC:
    case PPC_INST_VANDC128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_andnot_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VAVGSB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_avg_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VAVGSH:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_avg_epi16(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VAVGUB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_avg_epu8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VAVGUH:
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_avg_epu16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VCTSXS:
    case PPC_INST_VCFPSXWS128:
        printSetFlushMode(true);
        print("\t_mm_store_si128((__m128i*){}.s32, _mm_vctsxs(", v(insn.operands[0]));
        if (insn.operands[2] != 0)
            println("_mm_mul_ps(_mm_load_ps({}.f32), _mm_set1_ps({}))));", v(insn.operands[1]), 1u << insn.operands[2]);
        else
            println("_mm_load_ps({}.f32)));", v(insn.operands[1]));
        break;

    case PPC_INST_VCTUXS:
    case PPC_INST_VCFPUXWS128:
        printSetFlushMode(true);
        print("\t_mm_store_si128((__m128i*){}.u32, _mm_vctuxs(", v(insn.operands[0]));
        if (insn.operands[2] != 0)
            println("_mm_mul_ps(_mm_load_ps({}.f32), _mm_set1_ps({}))));", v(insn.operands[1]), 1u << insn.operands[2]);
        else
            println("_mm_load_ps({}.f32)));", v(insn.operands[1]));
        break;

    case PPC_INST_VCFSX:
    case PPC_INST_VCSXWFP128:
    {
        printSetFlushMode(true);
        print("\t_mm_store_ps({}.f32, ", v(insn.operands[0]));
        if (insn.operands[2] != 0)
        {
            const float value = ldexp(1.0f, -int32_t(insn.operands[2]));
            println("_mm_mul_ps(_mm_cvtepi32_ps(_mm_load_si128((__m128i*){}.u32)), _mm_castsi128_ps(_mm_set1_epi32(int(0x{:X})))));", v(insn.operands[1]), *reinterpret_cast<const uint32_t*>(&value));
        }
        else
        {
            println("_mm_cvtepi32_ps(_mm_load_si128((__m128i*){}.u32)));", v(insn.operands[1]));
        }
        break;

    case PPC_INST_VCFUX:
    case PPC_INST_VCUXWFP128:
    {
        printSetFlushMode(true);
        print("\t_mm_store_ps({}.f32, ", v(insn.operands[0]));
        if (insn.operands[2] != 0)
        {
            const float value = ldexp(1.0f, -int32_t(insn.operands[2]));
            println("_mm_mul_ps(_mm_cvtepu32_ps_(_mm_load_si128((__m128i*){}.u32)), _mm_castsi128_ps(_mm_set1_epi32(int(0x{:X})))));", v(insn.operands[1]), *reinterpret_cast<const uint32_t*>(&value));
        }
        else
        {
            println("_mm_cvtepu32_ps_(_mm_load_si128((__m128i*){}.u32)));", v(insn.operands[1]));
        }
        break;

    case PPC_INST_VCMPBFP:
    case PPC_INST_VCMPBFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_vcmpbfp(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.setFromMask(_mm_load_ps({}.f32), 0xF);", cr(6), v(insn.operands[0]));
        break;

    case PPC_INST_VCMPEQFP:
    case PPC_INST_VCMPEQFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_cmpeq_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.setFromMask(_mm_load_ps({}.f32), 0xF);", cr(6), v(insn.operands[0]));
        break;

    case PPC_INST_VCMPEQUB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_cmpeq_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.setFromMask(_mm_load_si128((__m128i*){}.u8), 0xFFFF);", cr(6), v(insn.operands[0]));
        break;

    case PPC_INST_VCMPEQUW:
    case PPC_INST_VCMPEQUW128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_cmpeq_epi32(_mm_load_si128((__m128i*){}.u32), _mm_load_si128((__m128i*){}.u32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.setFromMask(_mm_load_ps({}.f32), 0xF);", cr(6), v(insn.operands[0]));
        break;

    case PPC_INST_VCMPGEFP:
    case PPC_INST_VCMPGEFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_cmpge_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.setFromMask(_mm_load_ps({}.f32), 0xF);", cr(6), v(insn.operands[0]));
        break;

    case PPC_INST_VCMPGTFP:
    case PPC_INST_VCMPGTFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_cmpgt_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.setFromMask(_mm_load_ps({}.f32), 0xF);", cr(6), v(insn.operands[0]));
        break;

    case PPC_INST_VCMPGTUB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_cmpgt_epu8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VCMPGTUH:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_cmpgt_epu16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VEXPTEFP:
    case PPC_INST_VEXPTEFP128:
        // TODO: vectorize
        printSetFlushMode(true);
        for (size_t i = 0; i < 4; i++)
            println("\t{}.f32[{}] = exp2f({}.f32[{}]);", v(insn.operands[0]), i, v(insn.operands[1]), i);
        break;

    case PPC_INST_VLOGEFP:
    case PPC_INST_VLOGEFP128:
        // TODO: vectorize
        printSetFlushMode(true);
        for (size_t i = 0; i < 4; i++)
            println("\t{}.f32[{}] = log2f({}.f32[{}]);", v(insn.operands[0]), i, v(insn.operands[1]), i);
        break;

    case PPC_INST_VMADDCFP128:
    case PPC_INST_VMADDFP:
    case PPC_INST_VMADDFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_add_ps(_mm_mul_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]), v(insn.operands[3]));
        break;

    case PPC_INST_VMAXFP:
    case PPC_INST_VMAXFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_max_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMAXSH:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_max_epi16(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMAXSW:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_max_epi32(_mm_load_si128((__m128i*){}.u32), _mm_load_si128((__m128i*){}.u32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMAXUH:
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_max_epu16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMINFP:
    case PPC_INST_VMINFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_min_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMINSH:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_min_epi16(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMINUH:
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_min_epu16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMRGHB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_unpackhi_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VMRGHH:
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_unpackhi_epi16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VMRGHW:
    case PPC_INST_VMRGHW128:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_unpackhi_epi32(_mm_load_si128((__m128i*){}.u32), _mm_load_si128((__m128i*){}.u32)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VMRGLB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_unpacklo_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VMRGLH:
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_unpacklo_epi16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VMRGLW:
    case PPC_INST_VMRGLW128:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_unpacklo_epi32(_mm_load_si128((__m128i*){}.u32), _mm_load_si128((__m128i*){}.u32)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VMSUM3FP128:
        // NOTE: accounting for full vector reversal here. should dot product yzw instead of xyz
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_dp_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32), 0xEF));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMSUM4FP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_dp_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32), 0xFF));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VMULFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_mul_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VNMSUBFP:
    case PPC_INST_VNMSUBFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_xor_ps(_mm_sub_ps(_mm_mul_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)), _mm_load_ps({}.f32)), _mm_castsi128_ps(_mm_set1_epi32(int(0x80000000)))));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]), v(insn.operands[3]));
        break;

    case PPC_INST_VNOR:
    case PPC_INST_VNOR128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_xor_si128(_mm_or_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)), _mm_set1_epi32(-1)));",
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VOR:
    case PPC_INST_VOR128:
        print("\t_mm_store_si128((__m128i*){}.u8, ", v(insn.operands[0]));

        if (insn.operands[1] != insn.operands[2])
            println("_mm_or_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[1]), v(insn.operands[2]));
        else
            println("_mm_load_si128((__m128i*){}.u8));", v(insn.operands[1]));

        break;

    case PPC_INST_VPERM:
    case PPC_INST_VPERM128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_perm_epi8_(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]), v(insn.operands[3]));
        break;

    case PPC_INST_VPERMWI128:
    {
        // NOTE: accounting for full vector reversal here
        uint32_t x = 3 - (insn.operands[2] & 0x3);
        uint32_t y = 3 - ((insn.operands[2] >> 2) & 0x3);
        uint32_t z = 3 - ((insn.operands[2] >> 4) & 0x3);
        uint32_t w = 3 - ((insn.operands[2] >> 6) & 0x3);
        uint32_t perm = x | (y << 2) | (z << 4) | (w << 6);
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_shuffle_epi32(_mm_load_si128((__m128i*){}.u32), 0x{:X}));", v(insn.operands[0]), v(insn.operands[1]), perm);
        break;

    case PPC_INST_VPKD3D128:
        // TODO: vectorize somehow?
        // NOTE: handling vector reversal here too
        printSetFlushMode(true);
        switch (insn.operands[2])
        {
        case 0: // D3D color
            if (insn.operands[3] != 1)
                fmt::println("Unexpected D3D color pack instruction at {:X}", base);

            for (size_t i = 0; i < 4; i++)
            {
                constexpr size_t indices[] = { 3, 0, 1, 2 };
                println("\t{}.u32[{}] = 0x404000FF;", vTemp(), i);
                println("\t{}.f32[{}] = {}.f32[{}] < 3.0f ? 3.0f : ({}.f32[{}] > {}.f32[{}] ? {}.f32[{}] : {}.f32[{}]);", vTemp(), i, v(insn.operands[1]), i, v(insn.operands[1]), i, vTemp(), i, vTemp(), i, v(insn.operands[1]), i);
                println("\t{}.u32 {}= uint32_t({}.u8[{}]) << {};", temp(), i == 0 ? "" : "|", vTemp(), i * 4, indices[i] * 8);
            }
            println("\t{}.u32[{}] = {}.u32;", v(insn.operands[0]), insn.operands[4], temp());
            break;

            break;

            break;

        break;

    case PPC_INST_VPKSHSS:
    case PPC_INST_VPKSHSS128:
        println("\t_mm_store_si128((__m128i*){}.s8, _mm_packs_epi16(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16)));", 
            v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VPKSHUS:
    case PPC_INST_VPKSHUS128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_packus_epi16(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16)));", v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VPKSWSS:
    case PPC_INST_VPKSWSS128:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_packs_epi32(_mm_load_si128((__m128i*){}.s32), _mm_load_si128((__m128i*){}.s32)));", 
            v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VPKSWUS:
    case PPC_INST_VPKSWUS128:
        println("\t_mm_store_si128((__m128i*){}.s32, _mm_load_si128((__m128i*){}.s32));", vTemp(), v(insn.operands[2]));
        for (int i = 0; i < 4; i++) {
            println("\t{}.u16[{}] = {}.s32[{}] < 0 ? 0 : ({}.s32[{}] > 0xFFFF ? 0xFFFF : {}.s32[{}]);",
                v(insn.operands[0]), i, vTemp(), i, vTemp(), i, vTemp(), i);
        }
        println("\t_mm_store_si128((__m128i*){}.s32, _mm_load_si128((__m128i*){}.s32));", vTemp(), v(insn.operands[1]));
        for (int i = 0; i < 4; i++) {
            println("\t{}.u16[{}] = {}.s32[{}] < 0 ? 0 : ({}.s32[{}] > 0xFFFF ? 0xFFFF : {}.s32[{}]);",
                v(insn.operands[0]), i + 4, vTemp(), i, vTemp(), i, vTemp(), i);
        }
        break;

    case PPC_INST_VPKUHUM:
        // Pack without saturation - use shuffle to select lower bytes
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_packus_epi16("
            "_mm_and_si128(_mm_load_si128((__m128i*){}.u16), _mm_set1_epi16(0xFF)), "
            "_mm_and_si128(_mm_load_si128((__m128i*){}.u16), _mm_set1_epi16(0xFF))));",
            v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VPKUHUS:
    case PPC_INST_VPKUHUS128:
        // Pack unsigned halfwords to unsigned bytes with saturation
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_packus_epi16(_mm_load_si128((__m128i*){}.u16), _mm_load_si128((__m128i*){}.u16)));", 
            v(insn.operands[0]), v(insn.operands[2]), v(insn.operands[1]));
        break;

    case PPC_INST_VPKUWUM:
    case PPC_INST_VPKUWUM128:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_load_si128((__m128i*){}.u32));", vTemp(), v(insn.operands[2]));
        for (int i = 0; i < 4; i++) {
            println("\t{}.u16[{}] = {}.u16[{}];", 
                v(insn.operands[0]), i, vTemp(), i*2);
        }
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_load_si128((__m128i*){}.u32));", vTemp(), v(insn.operands[1]));
        for (int i = 0; i < 4; i++) {
            println("\t{}.u16[{}] = {}.u16[{}];", 
                v(insn.operands[0]), i + 4, vTemp(), i*2);
        }
        break;

    case PPC_INST_VPKUWUS:
    case PPC_INST_VPKUWUS128:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_load_si128((__m128i*){}.u32));", vTemp(), v(insn.operands[2]));
        for (int i = 0; i < 4; i++) {
            println("\t{}.u16[{}] = {}.u32[{}] > 0xFFFF ? 0xFFFF : {}.u32[{}];",
                v(insn.operands[0]), i, vTemp(), i, vTemp(), i);
        }
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_load_si128((__m128i*){}.u32));", vTemp(), v(insn.operands[1]));
        for (int i = 0; i < 4; i++) {
            println("\t{}.u16[{}] = {}.u32[{}] > 0xFFFF ? 0xFFFF : {}.u32[{}];",
                v(insn.operands[0]), i + 4, vTemp(), i, vTemp(), i);
        }
        break;

    case PPC_INST_VREFP:
    case PPC_INST_VREFP128:
        // TODO: see if we can use rcp safely
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_div_ps(_mm_set1_ps(1), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VRFIM:
    case PPC_INST_VRFIM128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_round_ps(_mm_load_ps({}.f32), _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VRFIN:
    case PPC_INST_VRFIN128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_round_ps(_mm_load_ps({}.f32), _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VRFIZ:
    case PPC_INST_VRFIZ128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_round_ps(_mm_load_ps({}.f32), _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VRLIMI128:
    {
        constexpr size_t shuffles[] = { _MM_SHUFFLE(3, 2, 1, 0), _MM_SHUFFLE(2, 1, 0, 3), _MM_SHUFFLE(1, 0, 3, 2), _MM_SHUFFLE(0, 3, 2, 1) };
        println("\t_mm_store_ps({}.f32, _mm_blend_ps(_mm_load_ps({}.f32), _mm_permute_ps(_mm_load_ps({}.f32), {}), {}));", v(insn.operands[0]), v(insn.operands[0]), v(insn.operands[1]), shuffles[insn.operands[3]], insn.operands[2]);
        break;

    case PPC_INST_VRSQRTEFP:
    case PPC_INST_VRSQRTEFP128:
        // TODO: see if we can use rsqrt safely
        // TODO: we can detect if the input is from a dot product and apply logic only on one value
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_div_ps(_mm_set1_ps(1), _mm_sqrt_ps(_mm_load_ps({}.f32))));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VSEL:
    case PPC_INST_VSEL128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_or_si128(_mm_andnot_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)), _mm_and_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8))));", v(insn.operands[0]), v(insn.operands[3]), v(insn.operands[1]), v(insn.operands[3]), v(insn.operands[2]));
        break;

    case PPC_INST_VSL:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_vsl(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VSLB:
        // TODO: vectorize
        for (size_t i = 0; i < 16; i++)
            println("\t{}.u8[{}] = {}.u8[{}] << ({}.u8[{}] & 0x7);", v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i);
        break;

    case PPC_INST_VSLH:
        // Vector shift left halfword
        for (size_t i = 0; i < 8; i++)
            println("\t{}.u16[{}] = {}.u16[{}] << ({}.u16[{}] & 0xF);", 
                v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i);
        break;

    case PPC_INST_VSRAH:
        // Vector shift right algebraic halfword
        for (size_t i = 0; i < 8; i++)
            println("\t{}.s16[{}] = {}.s16[{}] >> ({}.u16[{}] & 0xF);", 
                v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i);
        break;

    case PPC_INST_VSRH:
        // Vector shift right halfword
        for (size_t i = 0; i < 8; i++)
            println("\t{}.u16[{}] = {}.u16[{}] >> ({}.u16[{}] & 0xF);", 
                v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i);
        break;

    case PPC_INST_VRLH:
        // Vector rotate left halfword
        for (size_t i = 0; i < 8; i++)
            println("\t{}.u16[{}] = ({}.u16[{}] << ({}.u16[{}] & 0xF)) | "
                "({}.u16[{}] >> (16 - ({}.u16[{}] & 0xF)));", 
                v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i,
                v(insn.operands[1]), i, v(insn.operands[2]), i);
        break;

    case PPC_INST_VSLDOI:
    case PPC_INST_VSLDOI128:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_alignr_epi8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8), {}));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]), 16 - insn.operands[3]);
        break;

    case PPC_INST_VSLW:
    case PPC_INST_VSLW128:
        // TODO: vectorize, ensure endianness is correct
        for (size_t i = 0; i < 4; i++)
            println("\t{}.u32[{}] = {}.u32[{}] << ({}.u8[{}] & 0x1F);", v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i * 4);
        break;

    case PPC_INST_VSPLTB:
    {
        // NOTE: accounting for full vector reversal here
        uint32_t perm = 15 - insn.operands[2];
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_shuffle_epi8(_mm_load_si128((__m128i*){}.u8), _mm_set1_epi8(char(0x{:X}))));", v(insn.operands[0]), v(insn.operands[1]), perm);
        break;

    case PPC_INST_VSPLTH:
    {
        // NOTE: accounting for full vector reversal here
        uint32_t perm = 7 - insn.operands[2];
        perm = (perm * 2) | ((perm * 2 + 1) << 8);
        println("\t_mm_store_si128((__m128i*){}.u16, _mm_shuffle_epi8(_mm_load_si128((__m128i*){}.u16), _mm_set1_epi16(short(0x{:X}))));", v(insn.operands[0]), v(insn.operands[1]), perm);
        break;

    case PPC_INST_VSPLTISB:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_set1_epi8(char(0x{:X})));", v(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_VSPLTISH:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_set1_epi16(short({})));", 
            v(insn.operands[0]), int16_t(insn.operands[1]));
        break;

    case PPC_INST_VSPLTISW:
    case PPC_INST_VSPLTISW128:
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_set1_epi32(int(0x{:X})));", v(insn.operands[0]), insn.operands[1]);
        break;

    case PPC_INST_VSPLTW:
    case PPC_INST_VSPLTW128:
    {
        // NOTE: accounting for full vector reversal here
        uint32_t perm = 3 - insn.operands[2];
        perm |= (perm << 2) | (perm << 4) | (perm << 6);
        println("\t_mm_store_si128((__m128i*){}.u32, _mm_shuffle_epi32(_mm_load_si128((__m128i*){}.u32), 0x{:X}));", v(insn.operands[0]), v(insn.operands[1]), perm);
        break;

    case PPC_INST_VSR:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_vsr(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VSRAW:
    case PPC_INST_VSRAW128:
        // TODO: vectorize, ensure endianness is correct
        for (size_t i = 0; i < 4; i++)
            println("\t{}.s32[{}] = {}.s32[{}] >> ({}.u8[{}] & 0x1F);", v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i * 4);
        break;

    case PPC_INST_VSRW:
    case PPC_INST_VSRW128:
        // TODO: vectorize, ensure endianness is correct
        for (size_t i = 0; i < 4; i++)
            println("\t{}.u32[{}] = {}.u32[{}] >> ({}.u8[{}] & 0x1F);", v(insn.operands[0]), i, v(insn.operands[1]), i, v(insn.operands[2]), i * 4);
        break;

    case PPC_INST_VSUBFP:
    case PPC_INST_VSUBFP128:
        printSetFlushMode(true);
        println("\t_mm_store_ps({}.f32, _mm_sub_ps(_mm_load_ps({}.f32), _mm_load_ps({}.f32)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VSUBSHS:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_subs_epi16(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16)));", 
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VSUBSWS:
        // TODO: vectorize
        for (size_t i = 0; i < 4; i++)
        {
            println("\t{}.s64 = int64_t({}.s32[{}]) - int64_t({}.s32[{}]);", temp(), v(insn.operands[1]), i, v(insn.operands[2]), i);
            println("\t{}.s32[{}] = {}.s64 > INT_MAX ? INT_MAX : {}.s64 < INT_MIN ? INT_MIN : {}.s64;", v(insn.operands[0]), i, temp(), temp(), temp());
        }
        break;

    case PPC_INST_VSUBUBS:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_subs_epu8(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VSUBUHM:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_sub_epi16(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
        break;

    case PPC_INST_VUPKD3D128:
        // TODO: vectorize somehow?
        // NOTE: handling vector reversal here too
        switch (insn.operands[2] >> 2)
        {
        case 0: // D3D color
            for (size_t i = 0; i < 4; i++)
            {
                constexpr size_t indices[] = { 3, 0, 1, 2 };
                println("\t{}.u32[{}] = {}.u8[{}] | 0x3F800000;", vTemp(), i, v(insn.operands[1]), indices[i]);
            }
            println("\t{} = {};", v(insn.operands[0]), vTemp());
            break;

            break;

            break;

        break;

    case PPC_INST_VUPKHSB:
    case PPC_INST_VUPKHSB128:
        println("\t_mm_store_si128((__m128i*){}.s16, _mm_cvtepi8_epi16(_mm_unpackhi_epi64(_mm_load_si128((__m128i*){}.s8), _mm_load_si128((__m128i*){}.s8))));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[1]));
        break;

    case PPC_INST_VUPKHSH:
    case PPC_INST_VUPKHSH128:
        println("\t_mm_store_si128((__m128i*){}.s32, _mm_cvtepi16_epi32(_mm_unpackhi_epi64(_mm_load_si128((__m128i*){}.s16), _mm_load_si128((__m128i*){}.s16))));", v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[1]));
        break;

    case PPC_INST_VUPKLSB:
    case PPC_INST_VUPKLSB128:
        println("\t_mm_store_si128((__m128i*){}.s32, _mm_cvtepi8_epi16(_mm_load_si128((__m128i*){}.s16)));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VUPKLSH:
    case PPC_INST_VUPKLSH128:
        println("\t_mm_store_si128((__m128i*){}.s32, _mm_cvtepi16_epi32(_mm_load_si128((__m128i*){}.s16)));", v(insn.operands[0]), v(insn.operands[1]));
        break;

    case PPC_INST_VXOR:
    case PPC_INST_VXOR128:
        print("\t_mm_store_si128((__m128i*){}.u8, ", v(insn.operands[0]));

        if (insn.operands[1] != insn.operands[2])
            println("_mm_xor_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)));", v(insn.operands[1]), v(insn.operands[2]));
        else
            println("_mm_setzero_si128());");

        break;

    case PPC_INST_XOR:
        println("\t{}.u64 = {}.u64 ^ {}.u64;", r(insn.operands[0]), r(insn.operands[1]), r(insn.operands[2]));
        if (strchr(insn.opcode->name, '.'))
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), r(insn.operands[0]), xer());
        break;

    case PPC_INST_XORI:
        println("\t{}.u64 = {}.u64 ^ {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2]);
        break;

    case PPC_INST_XORIS:
        println("\t{}.u64 = {}.u64 ^ {};", r(insn.operands[0]), r(insn.operands[1]), insn.operands[2] << 16);
        break;

    case PPC_INST_LWZUX:
        println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
        println("\t{}.u64 = PPC_LOAD_U32({});", r(insn.operands[0]), ea());
        println("\t{}.u32 = {};", r(insn.operands[1]), ea());
        break;

    case PPC_INST_STHU:
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\t{}{}, {}.u16);", mmioStore() ? "PPC_MM_STORE_U16(" : "PPC_STORE_U16(", ea(), r(insn.operands[0]));
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_VSEL:
        println("\t_mm_store_si128((__m128i*){}.u8, _mm_or_si128(_mm_andnot_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8)), _mm_and_si128(_mm_load_si128((__m128i*){}.u8), _mm_load_si128((__m128i*){}.u8))));", v(insn.operands[0]), v(insn.operands[3]), v(insn.operands[1]), v(insn.operands[3]), v(insn.operands[2]));
        break;

    case PPC_INST_BSO:
        printConditionalBranch(false, "so");
        break;

    case PPC_INST_BSOLR:
        println("\tif ({}.so) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_BNS:
        printConditionalBranch(true, "so");
        break;

    case PPC_INST_BNSLR:
        println("\tif (!{}.so) return;", cr(insn.operands[0]));
        break;

    case PPC_INST_DCBST:
        // no op
        break;

    case PPC_INST_FNMADD:
        printSetFlushMode(false);
        println("\t{}.f64 = -std::fma({}.f64, {}.f64, {}.f64);", f(insn.operands[0]), f(insn.operands[1]), f(insn.operands[2]), f(insn.operands[3]));
        if (strchr(insn.opcode->name, '.'))
            println("\tctx.fpscr.setFlags({}.f64);", f(insn.operands[0]));
        break;

    case PPC_INST_STFSU:
        printSetFlushMode(false);
        println("\t{}.f32 = float({}.f64);", temp(), f(insn.operands[0]));
        println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
        println("\t{}{}, {}.u32);", mmioStore() ? "PPC_MM_STORE_U32(" : "PPC_STORE_U32(", ea(), temp());
        println("\t{}.u32 = {};", r(insn.operands[2]), ea());
        break;

    case PPC_INST_VMINSW:
        // no op
        break;

    case PPC_INST_FRSQRTE: 
           printSetFlushMode(false);
        println("\t{}.f64 = double(1.0f / sqrtf(float({}.f64)));", f(insn.operands[0]), f(insn.operands[1]));
        break;

    case PPC_INST_LHBRX:
        print("\t{}.u64 = __builtin_bswap16(PPC_LOAD_U16(", r(insn.operands[0]));
        if (insn.operands[1] != 0)
            print("{}.u32 + ", r(insn.operands[1]));
        println("{}.u32));", r(insn.operands[2]));
        break;

         case PPC_INST_BDZF:
         {
             constexpr std::string_view fields[] = { "lt", "gt", "eq", "so" };
             println("\t--{}.u64;", ctr());
             println("\tif ({}.u32 == 0 && !{}.{}) goto loc_{:X};", ctr(), cr(insn.operands[0] / 4), fields[insn.operands[0] % 4], insn.operands[1]);
             break;

         case PPC_INST_STBUX:
             println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
             println("\t{}{}, {}.u8);", mmioStore() ? "PPC_MM_STORE_U8(" : "PPC_STORE_U8(", ea(), r(insn.operands[0]));
             println("\t{}.u32 = {};", r(insn.operands[1]), ea());
             break;

         case PPC_INST_LFSU:
             printSetFlushMode(false);
             println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
             println("\t{}.u32 = PPC_LOAD_U32({});", temp(), ea());
             println("\t{}.u32 = {};", r(insn.operands[2]), ea());
             println("\t{}.f64 = double({}.f32);", f(insn.operands[0]), temp());
             break;

         case PPC_INST_LHZU:
             println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
             println("\t{}.u64 = PPC_LOAD_U16({});", r(insn.operands[0]), ea());
             println("\t{}.u32 = {};", r(insn.operands[2]), ea());
             break;

         case PPC_INST_LBZUX:
             println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
             println("\t{}.u64 = PPC_LOAD_U8({});", r(insn.operands[0]), ea());
             println("\t{}.u32 = {};", r(insn.operands[1]), ea());
             break;

         case PPC_INST_LFSUX:
             printSetFlushMode(false);
             println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
             println("\t{}.u32 = PPC_LOAD_U32({});", temp(), ea());
             println("\t{}.u32 = {};", r(insn.operands[1]), ea());
             println("\t{}.f64 = double({}.f32);", f(insn.operands[0]), temp());
             break;

         case PPC_INST_STHUX:
             println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
             println("\t{}{}, {}.u16);", mmioStore() ? "PPC_MM_STORE_U16(" : "PPC_STORE_U16(", ea(), r(insn.operands[0]));
             println("\t{}.u32 = {};", r(insn.operands[1]), ea());
             break;

         case PPC_INST_STDUX:
             println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
             println("\t{}{}, {}.u64);", mmioStore() ? "PPC_MM_STORE_U64(" : "PPC_STORE_U64(", ea(), r(insn.operands[0]));
             println("\t{}.u32 = {};", r(insn.operands[1]), ea());
             break;

         case PPC_INST_LFDU:
             printSetFlushMode(false);
             println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
             println("\t{}.u64 = PPC_LOAD_U64({});", r(insn.operands[0]), ea());
             println("\t{}.u32 = {};", r(insn.operands[2]), ea());
             break;

         case PPC_INST_LHAU:
             print("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
             print("\t{}.s64 = int16_t(PPC_LOAD_U16({}));", r(insn.operands[0]), ea());
             print("\t{}.u32 = {};", r(insn.operands[2]), ea());
             break;

         case PPC_INST_STFDU:
             printSetFlushMode(false);
             println("\t{} = {} + {}.u32;", ea(), int32_t(insn.operands[1]), r(insn.operands[2]));
             println("\t{}{}, {}.u64);", mmioStore() ? "PPC_MM_STORE_U64(" : "PPC_STORE_U64(", ea(), r(insn.operands[0]));
             println("\t{}.u32 = {};", r(insn.operands[2]), ea());
             break;

         case PPC_INST_LHZUX:
             println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
             println("\t{}.u64 = PPC_LOAD_U16({});", r(insn.operands[0]), ea());
             println("\t{}.u32 = {};", r(insn.operands[1]), ea());
             break;

    case PPC_INST_CROR:
    {
        constexpr std::string_view fields[] = { "lt", "gt", "eq", "so" };
        println("\t{}.{} = {}.{} | {}.{};", cr(insn.operands[0] / 4), fields[insn.operands[0] % 4], cr(insn.operands[1] / 4), fields[insn.operands[1] % 4], cr(insn.operands[2] / 4), fields[insn.operands[2] % 4]);
        break;

    case PPC_INST_CRORC:
    {
        constexpr std::string_view fields[] = { "lt", "gt", "eq", "so" };
        println("\t{}.{} = {}.{} | (~{}.{} & 1);", cr(insn.operands[0] / 4), fields[insn.operands[0] % 4], cr(insn.operands[1] / 4), fields[insn.operands[1] % 4], cr(insn.operands[2] / 4), fields[insn.operands[2] % 4]);
        break;

    case PPC_INST_EQV: {
        const auto& dst = r(insn.operands[0]);
        const auto& lhs = r(insn.operands[1]);
        const auto& rhs = r(insn.operands[2]);

        println("\t{} = simd::xor_i32({}, {});", dst, lhs, rhs);
        println("\t{} = simd::andnot_i64({}, -1);", dst, dst);

        if (insn.opcode->name && strchr(insn.opcode->name, '.')) {
            println("\t{}.compare<int32_t>({}.s32, 0, {});", cr(0), dst, xer());
        }
        break;

    case PPC_INST_LDUX:
        println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
        println("\t{}.u64 = PPC_LOAD_U64({});", r(insn.operands[0]), ea());
        println("\t{}.u32 = {};", r(insn.operands[1]), ea());
        break;

    case PPC_INST_LFDUX:
        printSetFlushMode(false);
        println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
        println("\t{}.u64 = PPC_LOAD_U64({});", r(insn.operands[0]), ea());
        println("\t{}.u32 = {};", r(insn.operands[1]), ea());
        break;

case PPC_INST_LVLX:
case PPC_INST_LVLX128: {
    println("\t{}.u32 = {}.u32 + {}.u32;", temp(), r(insn.operands[1]), r(insn.operands[2]));
    EmitLoadShuffled(v(insn.operands[0]), temp());
    break;

    case PPC_INST_STFSUX:
        printSetFlushMode(false);
        println("\t{}.f32 = float({}.f64);", temp(), f(insn.operands[0]));
        println("\t{} = {}.u32 + {}.u32;", ea(), r(insn.operands[1]), r(insn.operands[2]));
        println("\t{}{}, {}.u32);", mmioStore() ? "PPC_MM_STORE_U32(" : "PPC_STORE_U32(", ea(), temp());
        println("\t{}.u32 = {};", r(insn.operands[1]), ea());
        break;

case PPC_INST_STVEHX: {
    // Begin PPC_STORE_U16 call
    print("\tPPC_STORE_U16((");
    if (insn.operands[1] != 0)
        print("{}.u32 + ", r(insn.operands[1]));
    print("{}.u32) & ~0x1, ", r(insn.operands[2]));

    // Correct parenthesis balancing here
    print("simd::extract_u16(simd::to_vec128i({}), 7 - (((", v(insn.operands[0]));
    if (insn.operands[1] != 0)
        print("{}.u32 + ", r(insn.operands[1]));
    println("{}.u32) & 0xF) >> 1)));", r(insn.operands[2]));  // ← One extra closing paren added
    break;

case PPC_INST_STVEWX:
case PPC_INST_STVEWX128: {
    // Begin PPC_STORE_U32 call
    print("\tPPC_STORE_U32((");
    if (insn.operands[1] != 0)
        print("{}.u32 + ", r(insn.operands[1]));
    print("{}.u32) & ~0x3, ", r(insn.operands[2]));

    // Complete simd::extract_u32 call
    print("simd::extract_u32(*reinterpret_cast<const simd::vec128i*>(&{}.u32), 3 - ((", v(insn.operands[0]));
    if (insn.operands[1] != 0)
        print("{}.u32 + ", r(insn.operands[1]));
    println("{}.u32) & 0xF) >> 2));", r(insn.operands[2]));
    break;

case PPC_INST_STVLX:
case PPC_INST_STVLXL:
case PPC_INST_STVLX128:
case PPC_INST_STVLXL128: {
    println("{{"); // ← Start a scoped block
    println("\tuint32_t addr = ");
    if (insn.operands[1] != 0)
        print("{}.u32 + ", r(insn.operands[1]));
    println("{}.u32;", r(insn.operands[2]));

    println("\tuint32_t tmp_off = addr & 0xF;");
    println("\tfor (size_t i = 0; i < (16 - tmp_off); i++)");
    println("\t\tPPC_STORE_U8(addr + i, simd::extract_u8(simd::to_vec128i({}), 15 - i));", v(insn.operands[0]));
    println("}}");
    break;

case PPC_INST_STVRX:
case PPC_INST_STVRXL:
case PPC_INST_STVRX128:
case PPC_INST_STVRXL128: {
    println("{{");
    println("\tuint32_t addr = ");
    if (insn.operands[1] != 0)
        print("{}.u32 + ", r(insn.operands[1]));
    println("{}.u32;", r(insn.operands[2]));

    println("\tuint32_t tmp_off = addr & 0xF;");
    println("\tfor (size_t i = 0; i < tmp_off; i++)");
    println("\t\tPPC_STORE_U8(addr - i - 1, simd::extract_u8(simd::to_vec128i({}), i));", v(insn.operands[0]));
    println("}}");
    break;

case PPC_INST_STVX:
    printSetFlushMode(true);
    println("\tuint32_t addr{} = ({}.u32 + {}.u32) & ~0xF;", insn.operands[0], r(insn.operands[1] == 0 ? 0 : insn.operands[1]), r(insn.operands[2]));
    println("\t*(volatile uint32_t*)addr{} = {}.u32[0];", insn.operands[0], v(insn.operands[0]));
    println("\t*(volatile uint32_t*)(addr{} + 4) = {}.u32[1];", insn.operands[0], v(insn.operands[0]));
    println("\t*(volatile uint32_t*)(addr{} + 8) = {}.u32[2];", insn.operands[0], v(insn.operands[0]));
    println("\t*(volatile uint32_t*)(addr{} + 12) = {}.u32[3];", insn.operands[0], v(insn.operands[0]));
    break;

    case PPC_INST_STVEBX:
        // Compute 16-byte aligned effective address (as required by STVEX family)
        println("\t{} = ({}.u32 + {}.u32) & ~0xF;", ea(), r(insn.operands[1]), r(insn.operands[2]));

        // Compute the index within the vector (big-endian style)
        println("\tmem::store8({}.u8[15 - (({}.u32 + {}.u32) & 0xF)], base[{}]);",
            v(insn.operands[0]),
            r(insn.operands[1]), r(insn.operands[2]),
            ea());
        break;

    case PPC_INST_MTCRF:
        printSetFlushMode(true);
        println("\tuint32_t mask = {};", insn.operands[0]);
        println("\tuint32_t value = {}.u32;", r(insn.operands[1]));
        println("\tfor (int i = 0; i < 8; ++i) {");
        println("\t\tif (mask & (1 << (7 - i))) {");
        println("\t\t\tuint32_t field = (value >> (i * 4)) & 0xF;");
        println("\t\t\tstate.cr &= ~(0xF << (i * 4));");
        println("\t\t\tstate.cr |= (field << (i * 4));");
        println("\t\t}");
        println("\t}");
        break;

    case PPC_INST_BGTLA:
        printSetFlushMode(true);
        // CR[BI]'s GT bit is at bit 1 in the 4-bit field: mask is 0b0100 = 0x4
        println("\tif ((state.cr & (0x4 << (4 * (7 - {})))) != 0) {{", insn.operands[0]);
        println("\t\tctx.lr = 0x{:X};  // Link to next instruction", base + 4);
        println("\t\treturn 0x{:X};    // Branch absolute", uint32_t(insn.operands[1]));
        println("\t}}");
        break;

    case PPC_INST_VSUBSBS:
        printSetFlushMode(true);
        println("\t{}.v128 = simd::sub_saturate_i8({}.v128, {}.v128);",
            v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
     break;

case PPC_INST_VSRAB: {
    printSetFlushMode(true);
    println("simd::store_shuffled({}, simd::shift_right_arithmetic_i8(simd::to_vec128i({}), simd::and_u8(simd::to_vec128i({}), simd::set1_i8(0x7))));",
        v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
    break;

case PPC_INST_VSRAH: {
    printSetFlushMode(true);
    println("simd::store_shuffled({}, simd::shift_right_arithmetic_i16(simd::to_vec128i({}), simd::and_u16(simd::to_vec128i({}), simd::set1_i16(0xF))));",
        v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
    break;

case PPC_INST_VSRAW:
case PPC_INST_VSRAW128: {
    printSetFlushMode(true);
    println("simd::store_shuffled({}, simd::shift_right_arithmetic_i32(simd::to_vec128i({}), simd::and_u32(simd::to_vec128i({}), simd::set1_i32(0x1F))));",
        v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
    break;

case PPC_INST_VSRH: {
    printSetFlushMode(true);
    println("simd::store_shuffled({}, simd::shift_right_logical_i16(simd::to_vec128i({}), simd::and_u16(simd::to_vec128i({}), simd::set1_i16(0xF))));",
        v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
    break;

case PPC_INST_VSRW:
case PPC_INST_VSRW128: {
    printSetFlushMode(true);
    println("simd::store_shuffled({}, simd::shift_right_logical_i32(simd::to_vec128i({}), simd::and_u32(simd::to_vec128i({}), simd::set1_i32(0x1F))));",
        v(insn.operands[0]), v(insn.operands[1]), v(insn.operands[2]));
    break;

