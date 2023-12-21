; day6.asm
; FORMAT: x86-64 NASM
; AUTHOR: Brent Yelle
; PURPOSE: Advent of Code 2023, Day 6: https://adventofcode.com/2023/day/6
; ASSEMBLING INSTRUCTIONS:
;    $ nasm -g -felf64 day6.asm
;    $ ld -g day6.o
;    $ ./a.out

SECTION .bss
; temporary holders for floating-point values
temp_half_m: resq 1                             ; m/2
temp_halfsqrt_discriminant: resq 1              ; sqrt(m^2 - 4*r)/2
temp_plus: resq 1                               ; m/2 + sqrt(m^2 - 4*r)/2
temp_minus: resq 1                              ; m/2 - sqrt(m^2 - 4*r)/2
; temporary holders for FPU Control Words
temp_oldCW: resw 1                              ; holds original control word (to restore it later)
temp_newCW: resw 1                              ; holds new word (that we're switching in temporarily)
; temporary holders for rounded versions of `temp_plus` and `temp_minus`
temp_plus_int: resq 1                           ; ceiling(temp_plus - 1)
temp_minus_int: resq 1                          ; floor(temp_plus + 1)

SECTION .data
; to hold answer (a cumulative product), and its base-10 ASCII representation
answer_num: dq 1                                ; integer form
answer_num_str: db "____________________", 10   ; base-10 ASCII representation, right-justified, 21 bytes long (10 is a newline character)
ANSWER_NUM_STR_LEN: equ 21                      ; length of the above string
answer_num_str_offset: dq 0                     ; offset in the above string where the number starts
; to hold nicely-printed string to introduce answers for Part 1 and Part 2
display_string: db "Answer for Part _: "
DISPLAY_STRING_LEN: equ $-display_string        ; length of the above string
DISPLAY_STRING_DIGIT: equ 16                    ; position of the '_' in the above string, to replace with a digit

SECTION .rodata
; data for the test case of part 1
test_times: dq 7, 15, 30
test_records: dq 9, 40, 200
TEST_COUNT: equ 3
; data for part 1
real_times_part1: dq 38, 67, 76, 73
real_records_part1: dq 234, 1027, 1157, 1236
REAL_COUNT1: equ 4
; data for part 2
real_time_part2: dq 38677673
real_record_part2: dq 234102711571236
REAL_COUNT2: equ 1
; FPU control word masks, used to change rounding mode
ROUNDUP:   equ 0000100000000000b
ROUNDDOWN: equ 0000010000000000b
ROUNDMASK: equ 1111001111111111b
; constants for FPU operations in `raceWinLimits`, since we can't load immediates directly into FPU registers
TWO:  dq 2
FOUR: dq 4

global _start

SECTION .text
_start:
    nop
main_part1:
    mov BYTE [display_string + DISPLAY_STRING_DIGIT], '1'    ; set as "Part 1"
    mov rdi, real_times_part1       ; PARAM1 (RDI): pointer to qword array of maximum times
    mov rsi, real_records_part1     ; PARAM2 (RSI): pointer to qword array of records
    mov rdx, REAL_COUNT1            ; PARAM3 (RDX): length (in qwords) of arrays in RDI and RSI
    mov rcx, answer_num             ; PARAM4 (RCX): pointer to qword-location to store numerical result
    mov r8, answer_num_str          ; PARAM5 (R8) : pointer to 21-byte string to hold ASCII version of result
    mov r9, answer_num_str_offset   ; PARAM6 (R9) : pointer to qword-location to store offset in the above ASCII string
    call run_over_array
main_part2:
    mov BYTE [display_string + DISPLAY_STRING_DIGIT], '2'    ; set as "Part 2"
    mov rdi, real_time_part2        ; PARAM1 (RDI): pointer to qword array of maximum times
    mov rsi, real_record_part2      ; PARAM2 (RSI): pointer to qword array of records
    mov rdx, REAL_COUNT2            ; PARAM3 (RDX): length (in qwords) of arrays in RDI and RSI
    mov rcx, answer_num             ; PARAM4 (RCX): pointer to qword-location to store numerical result
    mov r8, answer_num_str          ; PARAM5 (R8) : pointer to 21-byte string to hold ASCII version of result
    mov r9, answer_num_str_offset   ; PARAM6 (R9) : pointer to qword-location to store offset in the above ASCII string
    call run_over_array
exit:
    mov rax, 60     ; sys_exit
    mov rdi, 0      ; exit code 0
    syscall


; -------------------------------
; itos: converts an integer to its base-10 form in printable ASCII. Number will be right-justified, be stored in first 20 bytes of the string pointed at by RSI, with leading places filled with spaces. The size of 20 bytes was chosen because the largest 64-bit unsigned integer, 2^64 - 1 = 18446744073709551615, is 20 decimal digits.
; -------------------------------
; PARAM1 (RDI): number to be converted (qword value)
; PARAM2 (RSI): address of string to hold converted number
; RETURN (RAX): offset of the number's first ASCII digit from the initial address (qword value)
; -------------------------------
itos:
    ; storing previous arguments
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r10
itos_fillspaces_setup:
    mov rbx, rsi                ; RBX holds address of array[0]
    mov rcx, 20                 ; RCX holds 20 (for the 20 digits of the number)
    mov r10, 10                 ; R10 holds 10 (for base-10)
itos_fillspaces_loop:
    ; fill the first 20 bytes of the given array with spaces
    ; for (count=20; count>0; count--)
    mov BYTE [rbx], ' '         ; array[i] = ' '  (starts at i=0)
    inc rbx                     ; i++
    loop itos_fillspaces_loop
itos_convert_setup:
    mov rbx, rsi                ; RBX holds address of array[0]
    add rbx, 20                 ; RBX holds address of array[20]
    mov rax, rdi                ; RAX holds copy of number to convert
itos_convert_loop:
    ; while (quotient != 0), i=20
    test rax, rax               ; if RAX == 0
    jz itos_afterloop           ; then exit loop, otherwise
    dec rbx                     ; i--
    cqo                         ; extend RAX to RDX:RAX
    idiv r10                    ; divide by 10, now RAX holds quotient and RDX holds remainder
    add rdx, '0'                ; RDX holds ASCII value of the corresponding digit
    mov [rbx], dl               ; store that ASCII value in array[i]
    jmp itos_convert_loop       ; otherwise, keep looping
itos_afterloop:
    ; at this point:
    ;   RAX holds 0
    ;   RBX holds address of leftmost bit of the number (right-justified)
    ;   RDX holds last-written bit (junk)
    ;   RDI holds the number that has been converted
    ;   RSI holds address of first bit in the string
    sub rbx, rsi                ; RBX now holds spacing offset of number from the left edge of the string
    mov rax, rbx                ; RAX now holds " " " "
    ; return previous values
    pop r10
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    ret

; -------------------------------
; raceWinLimits: If there is a set length of time `m`, then given an integer length of time x (0 <= x <= maxtime), the distance that we go in the race is `d(x) = x * (m - x)`. If there is a record `r` that we're trying to beat, we need to find the intersection points of `d(x) = r`, and the winning times will be all integers between (exclusive) these intersection points. We can achieve this exclusion by rounding appropriately.
; Solving the quadratic equation `x*(m-x) = r` for `x`, we get: `x = m/2 ± sqrt(m^2 - 4*r)/2`. First, we calculate these two solutions, we round them appropriately to get the integer limits `x_min` and `x_max`, then calculate the number of integers in that range `[x_min, x_max]` as `x_max - x_min + 1`.
; -------------------------------
; PARAM1 (RDI): pointer to maximum time allowed on a race (`m`)
; PARAM2 (RSI): pointer to record distance in the given race (`r`)
; RETURN (RAX): number of ways to beat the record (qword value)
; -------------------------------
raceWinLimits:
    ; store register backups
    push rdi
    push rsi
raceWinLimits_calc_halfsqrt_discriminant:
    ; calculate sqrt(m^2 - 4*r) / 2
    fild QWORD [rdi]                    ; ST(0) = m
    fild QWORD [rdi]                    ; setup to multiply m by m
    fmul                                ; ST(0) = m^2
    fild QWORD [rsi]                    ; ST(0) = r,   ST(1) = m^2
    fild QWORD [FOUR]                   ; setup to multiply r by 4
    fmulp                               ; ST(0) = 4*r, ST(1) = m^2
    fsubp                               ; ST(0) = m^2 - 4*r
    fsqrt                               ; ST(0) = sqrt(m^2 - 4*r)
    fild QWORD [TWO]                    ; setup to divide by 2
    fdivp                               ; ST(0) = sqrt(m^2 - 4*r) / 2
    ; copy ST(0) into memory
    fstp QWORD [temp_halfsqrt_discriminant] ; pop ST(0) and store into memory: this is half of the square root of the discriminant
    ; FPU stack is now empty
raceWinLimits_calc_half_m:
    ; calculate m/2
    fild QWORD [rdi]                    ; ST(0) = m
    fild QWORD [TWO]                    ; setup to divide by 2
    fdivp                               ; ST(0) = m/2
    fstp QWORD [temp_half_m]            ; pop ST(0) and store into memory
    ; FPU stack is now empty
raceWinLimits_calc_plusminus:
    ; calculate + and - solutions of quadratic equation
    fld QWORD [temp_halfsqrt_discriminant]
    fld QWORD [temp_half_m]             ; ST(0) = m/2, ST(1) = sqrt(m^2 - 4*r)/2
    faddp                               ; ST(0) = m/2 + sqrt(m^2 - 4*r)/2
    fstp QWORD [temp_plus]              ; pop ST(0) and store into memory: this is the "plus-answer"
    ; FPU stack is now empty
    fld QWORD [temp_halfsqrt_discriminant]
    fld QWORD [temp_half_m]             ; ST(0) = m/2, ST(1) = sqrt(m^2 - 4*r)/2
    fsubrp                              ; ST(0) = m/2 - sqrt(m^2 - 4*r)/2
    fstp QWORD [temp_minus]             ; pop ST(0) and store into memory: this is the "minus-answer"
    ; FPU stack is now empty
raceWinLimits_round_setup:
    ; save previous FPU Control Word (FPU CW)
    fnstcw WORD [temp_oldCW]            ; copy to restore later
    fnstcw WORD [temp_newCW]            ; copy that we'll edit
raceWinLimits_round_plus:
    ; plus = ceiling(plus - 1)
    and WORD [temp_newCW], ROUNDMASK    ; zero-out the FPU CW's rounding control bits
    or WORD [temp_newCW], ROUNDUP       ; set the rounding control bits to 10b (round up)
    fldcw [temp_newCW]                  ; load our custom FPU CW
    fld QWORD [temp_plus]               ; ST(0) = plus_answer
    fld1                                ; setup to subtract 1
    fsubp                               ; ST(0) = plus_answer - 1
    fistp QWORD [temp_plus_int]         ; temp_plus_int = ceiling(plus_answer - 1), pop ST(0)
    ; FPU stack is now empty
raceWinLimits_round_minus:
    ; minus = floor(minus + 1)
    and WORD [temp_newCW], ROUNDMASK    ; zero-out the FPU CW's rounding control bits
    or WORD [temp_newCW], ROUNDDOWN     ; set the rounding control bits to 01b (round down)
    fldcw [temp_newCW]                  ; load our custom FPU CW
    fld QWORD [temp_minus]              ; ST(0) = minus_answer
    fld1                                ; setup to add 1
    faddp                               ; ST(0) = minus_answer + 1
    fistp QWORD [temp_minus_int]        ; temp_minus_int = floor(minus_answer + 1), pop ST(0)
    ; FPU stack is now empty
raceWinLimits_round_cleanup:
    ; restore original FPU control word
    fldcw [temp_oldCW]
raceWinLimits_get_range:
    mov rax, [temp_plus_int]            ; RAX = maximum
    sub rax, [temp_minus_int]           ; RAX = maximum - minimum
    inc rax                             ; RAX = maximum - minimum + 1
raceWinLimits_exit:
    ; restore pre-function state of registers
    pop rsi
    pop rdi
    ret

; -------------------------------
; run_over_array: Given two parallel arrays pointed at by RDI and RSI (containing maximum race times and records, respectively), this function iterates over every pair and runs `raceWinLimits` on them. Each result from calling `raceWinLimits` is then taken and multiplied into a cumulative product (initialized to 1). At the conclusion of all loop iterations, the cumulative product is printed.
; -------------------------------
; PARAM1 (RDI): pointer to qword array of maximum times
; PARAM2 (RSI): pointer to qword array of records
; PARAM3 (RDX): length (in qwords) of arrays in RDI and RSI
; PARAM4 (RCX): pointer to qword-location to store numerical result (cumulative product)
; PARAM5 (R8) : pointer to 21-byte string to hold ASCII version of result
; PARAM6 (R9) : pointer to qword-location to store offset in the above ASCII string
; RETURN (RAX): cumulative product (qword, value stored at address in RCX)
; -------------------------------
run_over_array:
    ; make backup of pre-function state
    push rdi
    push rsi
    push rdx
    push r15
    push rbx
    push rcx
    ; we need to use RCX for a lot, so move its value into R15
    mov r15, rcx        ; R15 is a duplicate of RCX's argument-value
roa_setup:
    mov r11, rdi        ; R11 will hold &max_times[i]
    mov r12, rsi        ; R12 will hold &records[i]
    mov rcx, rdx        ; RCX will hold loop counter (# of elements in parallel arrays above)
    mov QWORD [r15], 1  ; reset cumulative product to 1
roa_loopstart:
    mov rdi, r11        ; PARAM1: &max_times[i]
    mov rsi, r12        ; PARAM2: &records[i]
    call raceWinLimits
    ; RAX now holds the number of ways we could win 
roa_multiply:
    mov rbx, rax        ; RBX = number_of_ways_we_could_win[i]
    mov rax, [r15]      ; RAX = cumulative product of number_of_ways_we_could_win[i]
    imul rbx            ; RBX = new cumulative product
    mov [r15], rax      ; store cumulative product back in memory
    add r11, 8          ; R11 now holds &max_times[i+1]
    add r12, 8          ; R12 now holds &records[i+1]
    loop roa_loopstart
roa_showanswer1:
    ; convert cumulative product to a string
    mov rdi, [r15]      ; PARAM1: value to convert (cumulative product)
    mov rsi, r8         ; PARAM2: pointer to 21-byte string to hold the ASCII number
    call itos
    ; RAX holds the offset in the string to start printing at
    mov [r9], rax   ; store the offset of the string
    ; print the part 1header
    mov rax, 1                  ; sys_write 
    mov rdi, 1                  ; stdout
    mov rsi, display_string     ; fixed prompt string
    mov rdx, DISPLAY_STRING_LEN ; fixed prompt string's length
    syscall
roa_showanswer2:
    ; print the part 1 result
    mov rax, 1                  ; sys_write
    mov rdi, 1                  ; stdout
    mov rsi, r8                 ; RSI = pointer to ASCII string of number result
    add rsi, [r9]               ; shift pointer by the known offset (to get only the numeric chars & the newline)
    mov rdx, ANSWER_NUM_STR_LEN ; RDX = length of the above ascii string
    sub rdx, [r9]               ; shift length by the known offset (to not print past the end of the string)
    syscall
roa_exit:
    ; restore pre-function values
    pop rcx
    pop rbx
    pop r15
    pop rdx
    pop rsi
    pop rdi
    mov rax, [rcx]              ; RAX (return value): number stored at RCX (the cumulative product)
    ret
