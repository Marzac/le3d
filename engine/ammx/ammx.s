    xdef _prepare_fill_texel
    xdef _fill_flat_texel
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
; 56(a7) int* divCache
;
; additional offset because of saved registers: 24
;
_fill_flat_texel:
    movem.l d2-d7/a2,-(a7)
    move.l 28(a7),a1    ; p in a1
    move.l 36(a7),d5    ; u1 in d5
    move.l 60(a7),d6    ; v1 in d6
    move.l 44(a7),d7    ; w1 in d7
    bclr.l  #$09,d4     ; indicator if we should store our calculated result => start with 0
    move.l 32(a7),d0    ; d0 is our loopvar
    bra .loopend
.loopstart
    ; calculate z
    move.l d7,d2
    asr.l #8,d2
    divs.l d2,d1    ; z in d1
    move.b #24,d4   ; needed for shift right
    ; calculate tu
    move.l d5,d2
    move.l d6,d3
    muls.l d1,d2
    lsr.l d4,d2
    muls.l d1,d3
    lsr.l d4,d3
    and.l 60(a7),d2     ; tu in d2
    ; calculate tv
    and.l 64(a7),d3     ; tv in d3
    ; no idea why this is uint32_t :S
    move.b 71(a7),d4    ; texSizeU (68 + 3 => fetch only the byte part of the uint32_t)
    lsl.l d4,d3
    add.l d2,d3         ; texpixel offset in d3
    lsl.l #2,d3         ; it is a 32bit field => multiply by 4
    move.l 72(a7),d1
    add.l d3,d1
    move.l d1,a0        ; t
    ; calculate p[]
    load (a0),e0
    vperm #$48494a4b,e4,e0,e2
    bchg.l #$09,d4
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
    dbra d0,.loopstart
    ; XXX no handling for odd d!
.end:
    movem.l (a7)+,a2/d2-d7
    rts