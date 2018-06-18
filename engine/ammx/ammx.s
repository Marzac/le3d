    xdef _prepare_fill_texel
    xdef _fill_flat_texel_int
    xdef _fill_flat_texel_float
    xdef _set_ammx_pixels

; 4(a7) void* p
; 8(a7) size_t bytes
; 12(a7) uint32_t color
_set_ammx_pixels:
    move.l 4(a7),a0
    move.l 8(a7),d0
    move.l 12(a7),d1
    vperm #$cdefcdef,d0,d1,e0
.loop
    storec e0,d0,(a0)+
    subq.l #8,d0
    bgt    .loop
    rts

; 4(a7) LeColor* c
_prepare_fill_texel:
    load #3,e4
    move.l 4(a7),a0
    load (a0),e0
    vperm #$48494a4b,e4,e0,e1   ; c in e1
    rts

; 
; 4(a7) uint8_t* p
; 8(a7) short d
; 12(a7) int u1
; 16(a7) int v1
; 20(a7) int w1
; 24(a7) int au
; 28(a7) int av
; 32(a7) int aw
; 36(a7) uint32_t texMaskU
; 40(a7) uint32_t texMaskV
; 44(a7) uint32_t texSizeU
; 48(a7) uint32_t* texPixels
; 52(a7) uint8_t* color
;
; additional offset because of saved registers: 24
;
_fill_flat_texel_int:
    movem.l d2-d7,-(a7)
    move.l 28(a7),a1    ; p in a1
    move.l 36(a7),d5    ; u1 in d5
    move.l 40(a7),d6    ; v1 in d6
    move.l 44(a7),d7    ; w1 in d7
    move.l 32(a7),d0    ; 16 bit d0 is our loopvar
    swap d0             ; other 16 bits are used for something else, namely:
    bclr.l #$09,d0     ; indicator if we should store our calculated result => start with 0
    bra .loopend
.loopstart
    swap d0
    ; calculate z
    move.l #$40000000,d1
    move.l d7,d2
    asr.l #8,d2
    divs.l d2,d1    ; z in d1
    move.b #24,d0   ; needed for shift right
    ; calculate tu
    move.l d5,d2
    muls.l d1,d3:d2
    lsr.l d0,d2
    lsl.l #8,d3
    or.l d3,d2
    and.l 60(a7),d2     ; tu in d2

    ; calculate tv
    move.l d6,d3
    muls.l d1,d4:d3
    lsr.l d0,d3
    lsl.l #8,d4
    or.l d4,d3
    and.l 64(a7),d3     ; tv in d3

    ; no idea why this is uint32_t :S
    move.b 71(a7),d0    ; texSizeU (68 + 3 => fetch only the byte part of the uint32_t)
    lsl.l d0,d3
    add.l d2,d3         ; texpixel offset in d3
    lsl.l #2,d3         ; it is a 32bit field => multiply by 4
    move.l 72(a7),d1
    add.l d3,d1
    move.l d1,a0        ; t
    ; calculate p[]
    load (a0),e0
    vperm #$48494a4b,e4,e0,e2
    bchg.l #$09,d0
    bne.s .secondpass
    pmul88 e1,e2,e5
    bra .continue
.secondpass
    pmul88 e1,e2,e6
    ; finally store our combined result
    vperm #$13579bdf,e5,e6,e3
    store e3,(a1)+
.continue
    ; NEXT!
    add.l 48(a7),d5
    add.l 52(a7),d6
    add.l 56(a7),d7
.loopend:
    swap d0
    dbra d0,.loopstart
    ; XXX no handling for odd d!
.end:
    movem.l (a7)+,d2-d7
    rts

; 4(a7) uint8_t* p
; 8(a7) short d
; 12(a7) float df
; 16(a7) float u1
; 20(a7) float v1
; 24(a7) float w1
; 28(a7) float u2
; 32(a7) float v2
; 36(a7) float w2
; 40(a7) uint32_t texMaskU
; 44(a7) uint32_t texMaskV
; 48(a7) uint32_t texSizeU
; 52(a7) uint32_t* texPixels
;
; additional offset because of saved registers: (fp2-fp7 => 480 bit) => 72 + d2-d6 => 20 = 92
;
_fill_flat_texel_float:
    fmovem.x fp2-fp7,-(a7)
    movem.l d2-d6,-(a7)
    move.l 96(a7),a0
    move.w 102(a7),d0            ; word sized so normal offset +2
    fsmove.s 104(a7),fp0     ; dfloat
    fsmove.s 108(a7),fp1     ; u1
    fsmove.s 112(a7),fp2     ; v1
    fsmove.s 116(a7),fp3     ; w1
    fsmove.s 120(a7),fp4
    fsmove.s 124(a7),fp5
    fsmove.s 128(a7),fp6
    move.l 132(a7),d1       ; texMaskU
    move.l 136(a7),d2       ; texMaskV
    move.b 143(a7),d3       ; texSizeU (byte sized)

    subq.l #$4,a7             ; 4 byte stack

    fssub fp1,fp4
    fssub fp2,fp5
    fssub fp3,fp6
    
    fsdiv fp0,fp4 ; au
    fsdiv fp0,fp5 ; av
    fsdiv fp0,fp6 ; aw
    
    bclr.l #17,d0     ; indicator if we should store our calculated result => start with 0

    bra .loopend
.loopstart
    fmove.s #$3f800000,fp7 ; 1.0f
    fsdiv fp3,fp7   
    fmove.s fp7,(a7)        ; z on fp7+stack

    fsmul fp1,fp7
    fintrz.x fp7
    fmove.l fp7,d4
    and.l d1,d4             ; tu

    fsmove.s (a7),fp7   
    fsmul fp2,fp7
    fintrz.x fp7
    fmove.l fp7,d5
    and.l d2,d5             ; tv

    lsl.l d3,d5
    add.l d4,d5         ; texpixel offset in d5
    lsl.l #2,d5         ; it is a 32bit field => multiply by 4

    move.l 148(a7),d6   ; 144+4 more due to local stack size!
    add.l d5,d6
    move.l d6,a1        ; t
    ; calculate p[]
    load (a1),e0
    vperm #$48494a4b,e4,e0,e2
    bchg.l #17,d0
    bne.s .secondpass
    pmul88 e1,e2,e5
    bra .continue
.secondpass
    pmul88 e1,e2,e6
    ; finally store our combined result
    vperm #$13579bdf,e5,e6,e3
    store e3,(a0)+
.continue
    fsadd fp4,fp1
    fsadd fp5,fp2
    fsadd fp6,fp3
.loopend
    dbra d0,.loopstart
    btst.l #17,d0
    beq.s .end
    ; probably case for storeilm 
    vperm #$13579bdf,e5,e6,e3
    moveq #$4,d0
    storec e3,d0,(a0)+
.end
    addq.l #$4,a7
    move.l 144(a7),d0
    movem.l (a7)+,d2-d6
    fmovem.x (a7)+,fp2-fp7
    rts