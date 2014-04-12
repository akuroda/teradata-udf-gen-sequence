#include <string.h>
#include <stdio.h>
#define SQL_TEXT Latin_Text
#include "sqltypes_td.h"

#define UDF_OK "00000"

typedef struct {
    int ampno;
    int current;
    int amp_start;
    int amp_end;
    unsigned short nodeid;
    unsigned short ampid;
} local_ctx_t;


static void write_trace(char *trace_buf)
{
    void *argv[1];
    int length[1];

    argv[0] = trace_buf;
    length[0] = strlen(trace_buf);
    FNC_Trace_Write_DL(1, argv, length);
}


void assign_numbers(int start, int count, int num_amps, local_ctx_t *context)
{
    int count_per_amp = count / num_amps;
    int remain = count % num_amps;

    context->amp_start = start + context->ampno * count_per_amp;
    context->amp_end = context->amp_start + count_per_amp - 1;

    if (remain == 0)
        return;

    /* need adjust when remain != 0 */

    if (count_per_amp > 0) {
        if (context->ampno >= remain) {
            context->amp_start += remain;
            context->amp_end += remain;
        } else {
            context->amp_start += context->ampno;
            context->amp_end += context->ampno + 1;
        }
    } else {
        if (count == 1 && context->ampno == 0) {
            context->amp_start = start;
            context->amp_end = start;
        } else if (context->ampno < remain) {
            /* count < num_amps */
            context->amp_start = start + context->ampno;
            context->amp_end = start + context->ampno;
        }
    }

}

void gen_sequence(INTEGER *start_num, INTEGER *end_num, INTEGER *result,
                  int *indic_arg1, int *indic_arg2, int *indic_r,
                  char sqlstate[6],
                  SQL_TEXT func_name[129], SQL_TEXT specific[129],
                  SQL_TEXT error_message[257])
{
    FNC_Phase Phase;
    local_ctx_t *context;
    AMP_Info_t *amp_info;
    FNC_Node_Info_t *node_info;
    int i;
    char trace_buf[256] = { 0 };
    int count;

    if (FNC_GetPhase(&Phase) != TBL_MODE_CONST) {
        /* set sqlstate to an error and return */
        strcpy(sqlstate, "U0005");
        return;
    }

    sprintf(sqlstate, UDF_OK);
    node_info = FNC_TblGetNodeData();
    amp_info = FNC_AMPInfo();

    count = *end_num - *start_num + 1;

    sprintf(trace_buf, "NODE [%d] AMP [%d] Phase [%d]",
            amp_info->NodeId, amp_info->AMPId, Phase);
    write_trace(trace_buf);
    
    switch(Phase) {
    case TBL_PRE_INIT:
        sprintf(trace_buf, "Nodes [%d], AMPs [%d]",
                node_info->NumAMPNodes, node_info->NumAMPs);
        write_trace(trace_buf);
        break;
    case TBL_INIT:
        context = FNC_TblAllocCtx(sizeof(local_ctx_t));
        context->ampno = -1;
        for (i = 0; i < node_info->NumAMPs; i++) {
            if (node_info->AN[i].NodeId == amp_info->NodeId &&
                node_info->AN[i].AMPId == amp_info->AMPId) {
                context->ampno = i;
                break;
            }
        }
        context->nodeid = amp_info->NodeId;
        context->ampid = amp_info->AMPId;

        assign_numbers(*start_num, count, node_info->NumAMPs, context);
        
        context->current = context->amp_start;

        sprintf(trace_buf, "Nodes [%d], AMPs [%d] AMP# %d, start %d end %d",
                node_info->NumAMPNodes, node_info->NumAMPs,
                context->ampno, context->amp_start, context->amp_end);
        write_trace(trace_buf);
        break;
    case TBL_BUILD:
        context = FNC_TblGetCtx();
        
        indic_r = 0;
        if (context->current > context->amp_end)
            strcpy(sqlstate, "02000");
        *result = context->current;
        context->current += 1;
        break;
    case TBL_END:
        break;
    case TBL_ABORT:
        break;
    }
}
