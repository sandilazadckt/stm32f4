# ARM Cortex-M4 Floating-Point Unit (FPU) - সম্পূর্ণ ডেভেলপমেন্ট নোট

## সূচিপত্র
1. [ভূমিকা](#ভূমিকা)
2. [FPU এর বৈশিষ্ট্য](#fpu-এর-বৈশিষ্ট্য)
3. [আর্কিটেকচার](#আর্কিটেকচার)
4. [রেজিস্টার সেট](#রেজিস্টার-সেট)
5. [FPU কনফিগারেশন](#fpu-কনফিগারেশন)
6. [প্রোগ্রামিং গাইড](#প্রোগ্রামিং-গাইড)
7. [অপটিমাইজেশন টেকনিক](#অপটিমাইজেশন-টেকনিক)
8. [উদাহরণ কোড](#উদাহরণ-কোড)

---

## ভূমিকা

### FPU কি? 
Floating-Point Unit (FPU) হল একটি বিশেষায়িত হার্ডওয়্যার ইউনিট যা ফ্লোটিং-পয়েন্ট গণনা দ্রুত সম্পাদন করে। ARM Cortex-M4 প্রসেসরে একটি ঐচ্ছিক FPU থাকে যা IEEE 754 স্ট্যান্ডার্ড অনুসরণ করে।

### কেন FPU ব্যবহার করবেন?
- **দ্রুততা**: সফটওয়্যার ফ্লোটিং-পয়েন্টের চেয়ে ১০-৫০ গুণ দ্রুত
- **নির্ভুলতা**: হার্ডওয়্যার-ভিত্তিক গণনা
- **শক্তি সাশ্রয়**: কম সাইকেল = কম পাওয়ার খরচ

---

## FPU এর বৈশিষ্ট্য

### প্রধান বৈশিষ্ট্য
1. **IEEE 754 সমর্থন**:  Single-precision (32-bit) ফ্লোটিং-পয়েন্ট
2. **৩২টি রেজিস্টার**:  S0-S31 (single-precision) বা D0-D15 (double-precision view)
3. **SIMD সক্ষমতা**: নির্দিষ্ট অপারেশনের জন্য
4. **Fused MAC**:  Multiply-Accumulate অপারেশন

### সমর্থিত ডেটা টাইপ
- **Single-precision (float)**: 32-bit IEEE 754
- **Half-precision**:  সীমিত সমর্থন (কনভার্শন)

### অপারেশন সেট
- যোগ, বিয়োগ, গুণ, ভাগ
- বর্গমূল (VSQRT)
- Fused Multiply-Add/Subtract (VFMA, VFMS)
- তুলনা অপারেশন
- কনভার্শন (integer ↔ float)

---

## আর্কিটেকচার

### FPU ব্লক ডায়াগ্রাম

```
┌─────────────────────────────────────┐
│     ARM Cortex-M4 Core              │
│                                     │
│  ┌──────────┐      ┌────────────┐  │
│  │   ALU    │      │    FPU     │  │
│  │          │      │  (FPv4-SP) │  │
│  └──────────┘      └────────────┘  │
│                          │          │
│  ┌───────────────────────┴────────┐ │
│  │   Register Bank (S0-S31)      │ │
│  └────────────────────────────────┘ │
└─────────────────────────────────────┘
```

### পাইপলাইন
FPU অপারেশনগুলি সাধারণত পাইপলাইন করা হয়:
- **১-সাইকেল**:  রেজিস্টার transfer
- **৩-সাইকেল**: যোগ/বিয়োগ, গুণ
- **১৪-সাইকেল**:  ভাগ (32-bit)
- **১৪-সাইকেল**:  বর্গমূল

---

## রেজিস্টার সেট

### ফ্লোটিং-পয়েন্ট রেজিস্টার

```
S0-S31:  32টি single-precision রেজিস্টার (32-bit প্রতিটি)
D0-D15:  16টি double-precision view (64-bit প্রতিটি)

ম্যাপিং: 
D0  = S1: S0
D1  = S3:S2
D15 = S31:S30
```

### কন্ট্রোল রেজিস্টার

#### 1. FPCCR (Floating-Point Context Control Register)
```c
// অ্যাড্রেস: 0xE000EF34
typedef struct {
    uint32_t LSPACT   : 1;  // Lazy State Preservation Active
    uint32_t USER     : 1;  // User mode privilege
    uint32_t Reserved :  1;
    uint32_t THREAD   : 1;  // Thread mode
    uint32_t HFRDY    : 1;  // Hard Fault Ready
    uint32_t MMRDY    : 1;  // Memory Management Fault Ready
    uint32_t BFRDY    : 1;  // Bus Fault Ready
    uint32_t Reserved2:  1;
    uint32_t MONRDY   : 1;  // Debug Monitor Ready
    uint32_t Reserved3: 21;
    uint32_t LSPEN    : 1;  // Lazy State Preservation Enable
    uint32_t ASPEN    : 1;  // Automatic State Preservation Enable
} FPCCR_Type;
```

#### 2. FPCAR (Floating-Point Context Address Register)
```c
// অ্যাড্রেস: 0xE000EF38
// FPU কনটেক্সট স্ট্যাক পয়েন্টার
```

#### 3. FPDSCR (Floating-Point Default Status Control Register)
```c
// অ্যাড্রেস: 0xE000EF3C
typedef struct {
    uint32_t Reserved : 22;
    uint32_t RMode    : 2;   // Rounding Mode
    uint32_t FZ       : 1;   // Flush-to-Zero
    uint32_t DN       : 1;   // Default NaN
    uint32_t AHP      : 1;   // Alternative Half-Precision
    uint32_t Reserved2: 5;
} FPDSCR_Type;
```

#### 4. FPSCR (Floating-Point Status and Control Register)
```c
// প্রোগ্রামেটিক অ্যাক্সেস (VMRS/VMSR দিয়ে)
typedef struct {
    uint32_t IOC      : 1;   // Invalid Operation
    uint32_t DZC      : 1;   // Divide by Zero
    uint32_t OFC      : 1;   // Overflow
    uint32_t UFC      : 1;   // Underflow
    uint32_t IXC      : 1;   // Inexact
    uint32_t Reserved : 2;
    uint32_t IDC      : 1;   // Input Denormal
    uint32_t IOE      : 1;   // Invalid Operation Enable
    uint32_t DZE      : 1;   // Divide by Zero Enable
    uint32_t OFE      : 1;   // Overflow Enable
    uint32_t UFE      : 1;   // Underflow Enable
    uint32_t IXE      :  1;   // Inexact Enable
    uint32_t Reserved2: 2;
    uint32_t IDE      : 1;   // Input Denormal Enable
    uint32_t Reserved3: 6;
    uint32_t RMode    : 2;   // Rounding Mode
    uint32_t FZ       : 1;   // Flush-to-Zero
    uint32_t DN       : 1;   // Default NaN
    uint32_t AHP      : 1;   // Alternative Half-Precision
    uint32_t Reserved4: 1;
    uint32_t V        : 1;   // Overflow condition flag
    uint32_t C        : 1;   // Carry condition flag
    uint32_t Z        : 1;   // Zero condition flag
    uint32_t N        : 1;   // Negative condition flag
} FPSCR_Type;
```

---

## FPU কনফিগারেশন

### ১. FPU সক্রিয় করা

```c
// CMSIS ব্যবহার করে
void FPU_Enable(void) {
    // FPU অ্যাক্সেস এনাবল করুন (CP10 এবং CP11)
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));  
    // CP10 এবং CP11 Full Access সেট করুন
    
    __DSB();  // Data Synchronization Barrier
    __ISB();  // Instruction Synchronization Barrier
}
```

### ২.  Lazy Context Switching কনফিগার করা

```c
void FPU_LazyContextSwitch_Enable(void) {
    // Lazy State Preservation সক্রিয় করুন
    FPU->FPCCR |= FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk;
}

void FPU_LazyContextSwitch_Disable(void) {
    // দ্রুত interrupt response এর জন্য
    FPU->FPCCR &= ~(FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk);
}
```

### ৩. Rounding Mode সেট করা

```c
typedef enum {
    FPU_ROUND_NEAREST = 0,  // Round to Nearest (ডিফল্ট)
    FPU_ROUND_PLUS_INF = 1, // Round toward +∞
    FPU_ROUND_MINUS_INF = 2,// Round toward -∞
    FPU_ROUND_ZERO = 3      // Round toward 0
} FPU_RoundMode_t;

void FPU_SetRoundingMode(FPU_RoundMode_t mode) {
    uint32_t fpscr;
    __ASM volatile("VMRS %0, FPSCR" : "=r" (fpscr));
    fpscr &= ~(3UL << 22);  // Clear RMode bits
    fpscr |= (mode << 22);   // Set new mode
    __ASM volatile("VMSR FPSCR, %0" : : "r" (fpscr));
}
```

### ৪. Exception Handling কনফিগার করা

```c
void FPU_EnableExceptions(void) {
    uint32_t fpscr;
    __ASM volatile("VMRS %0, FPSCR" : "=r" (fpscr));
    
    // Exception enable করুন
    fpscr |= (1 << 8)  |  // IOE - Invalid Operation
             (1 << 9)  |  // DZE - Divide by Zero
             (1 << 10) |  // OFE - Overflow
             (1 << 11) |  // UFE - Underflow
             (1 << 12);   // IXE - Inexact
    
    __ASM volatile("VMSR FPSCR, %0" : : "r" (fpscr));
}
```

---

## প্রোগ্রামিং গাইড

### C প্রোগ্রামিং

#### ১. মৌলিক ফ্লোটিং-পয়েন্ট অপারেশন

```c
#include <math.h>

// সাধারণ গণনা
float calculate_distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx*dx + dy*dy);  // FPU দ্বারা অপটিমাইজ হবে
}

// ট্রিগনোমেট্রিক ফাংশন
float calculate_angle(float opposite, float adjacent) {
    return atan2f(opposite, adjacent);
}

// Fused Multiply-Add ব্যবহার
float fma_example(float a, float b, float c) {
    return fmaf(a, b, c);  // (a * b) + c - একক অপারেশন
}
```

#### ২. ভেক্টর ম্যাথ

```c
// 3D ভেক্টর স্ট্রাকচার
typedef struct {
    float x, y, z;
} Vector3_t;

// ডট প্রোডাক্ট
float Vector3_Dot(const Vector3_t *a, const Vector3_t *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

// ক্রস প্রোডাক্ট
Vector3_t Vector3_Cross(const Vector3_t *a, const Vector3_t *b) {
    Vector3_t result;
    result.x = a->y * b->z - a->z * b->y;
    result.y = a->z * b->x - a->x * b->z;
    result.z = a->x * b->y - a->y * b->x;
    return result;
}

// নর্মালাইজেশন
void Vector3_Normalize(Vector3_t *v) {
    float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (length > 0.0f) {
        float inv_length = 1.0f / length;
        v->x *= inv_length;
        v->y *= inv_length;
        v->z *= inv_length;
    }
}
```

#### ৩. ম্যাট্রিক্স অপারেশন

```c
// 3x3 ম্যাট্রিক্স
typedef struct {
    float m[3][3];
} Matrix3x3_t;

// ম্যাট্রিক্স গুণ
void Matrix3x3_Multiply(const Matrix3x3_t *a, const Matrix3x3_t *b, 
                        Matrix3x3_t *result) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            float sum = 0.0f;
            for (int k = 0; k < 3; k++) {
                sum = fmaf(a->m[i][k], b->m[k][j], sum);  // FMA ব্যবহার
            }
            result->m[i][j] = sum;
        }
    }
}

// ম্যাট্রিক্স-ভেক্টর গুণ
Vector3_t Matrix3x3_Transform(const Matrix3x3_t *m, const Vector3_t *v) {
    Vector3_t result;
    result.x = m->m[0][0]*v->x + m->m[0][1]*v->y + m->m[0][2]*v->z;
    result. y = m->m[1][0]*v->x + m->m[1][1]*v->y + m->m[1][2]*v->z;
    result.z = m->m[2][0]*v->x + m->m[2][1]*v->y + m->m[2][2]*v->z;
    return result;
}
```

### Assembly প্রোগ্রামিং

#### ১. মৌলিক FPU নির্দেশনা

```asm
; যোগ
VADD.F32 S2, S0, S1      ; S2 = S0 + S1

; বিয়োগ
VSUB.F32 S2, S0, S1      ; S2 = S0 - S1

; গুণ
VMUL.F32 S2, S0, S1      ; S2 = S0 * S1

; ভাগ
VDIV. F32 S2, S0, S1      ; S2 = S0 / S1

; বর্গমূল
VSQRT.F32 S1, S0         ; S1 = √S0

; Fused Multiply-Add
VFMA.F32 S2, S0, S1      ; S2 = S2 + (S0 * S1)

; Fused Multiply-Subtract
VFMS.F32 S2, S0, S1      ; S2 = S2 - (S0 * S1)
```

#### ২. ডেটা ট্রান্সফার

```asm
; মেমরি থেকে লোড
VLDR.32 S0, [R0]         ; S0 = *R0
VLDM R0, {S0-S3}         ; একাধিক রেজিস্টার লোড

; মেমরিতে স্টোর
VSTR.32 S0, [R1]         ; *R1 = S0
VSTM R1, {S0-S3}         ; একাধিক রেজিস্টার স্টোর

; রেজিস্টার ট্রান্সফার (ARM ↔ FPU)
VMOV S0, R0              ; S0 = R0
VMOV R0, S0              ; R0 = S0

; FPSCR অ্যাক্সেস
VMRS R0, FPSCR           ; R0 = FPSCR
VMSR FPSCR, R0           ; FPSCR = R0
```

#### ৩. কনভার্শন নির্দেশনা

```asm
; Integer to Float
VCVT.F32.S32 S0, S0      ; signed int → float
VCVT.F32.U32 S0, S0      ; unsigned int → float

; Float to Integer
VCVT.S32.F32 S0, S0      ; float → signed int
VCVT.U32.F32 S0, S0      ; float → unsigned int

; Fixed-point conversion
VCVT.F32.S32 S0, S0, #16 ; Q16 fixed → float
VCVT.S32.F32 S0, S0, #16 ; float → Q16 fixed
```

#### ৪. তুলনা অপারেশন

```asm
; তুলনা এবং ফ্ল্যাগ সেট
VCMP.F32 S0, S1          ; S0 এবং S1 তুলনা করুন
VMRS APSR_nzcv, FPSCR    ; ফ্ল্যাগ ARM core এ ট্রান্সফার করুন

; conditional execution
BEQ equal_label          ; যদি সমান হয়
BGT greater_label        ; যদি বড় হয়
BLT less_label           ; যদি ছোট হয়
```

---

## অপটিমাইজেশন টেকনিক

### ১. Compiler অপটিমাইজেশন

```c
// GCC/ARM Compiler flags
// -mfpu=fpv4-sp-d16        :  FPU টাইপ নির্দিষ্ট করুন
// -mfloat-abi=hard         : হার্ড FPU ব্যবহার করুন
// -O2 বা -O3               : অপটিমাইজেশন লেভেল

// example compilation: 
// arm-none-eabi-gcc -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O2
```

### ২. কোড অপটিমাইজেশন

```c
// ভালো:  FMA ব্যবহার করুন
float result = fmaf(a, b, c);  // (a*b) + c - একক নির্দেশনা

// খারাপ: আলাদা অপারেশন
float result = (a * b) + c;    // দুটি নির্দেশনা

// ভালো: reciprocal ব্যবহার করুন
float inv_length = 1.0f / length;
x *= inv_length;
y *= inv_length;
z *= inv_length;

// খারাপ: বারবার ভাগ
x /= length;
y /= length;
z /= length;
```

### ৩. মেমরি অ্যালাইনমেন্ট

```c
// ৪-byte অ্যালাইন করা ডেটা দ্রুত
__attribute__((aligned(4))) float data_array[100];

// VLDM/VSTM এর জন্য ৮-byte অ্যালাইনমেন্ট উত্তম
__attribute__((aligned(8))) float aligned_data[100];
```

### ৪. Loop Unrolling

```c
// সাধারণ loop
for (int i = 0; i < n; i++) {
    result[i] = a[i] * b[i] + c[i];
}

// Unrolled loop (4x)
int i;
for (i = 0; i < n-3; i+=4) {
    result[i+0] = fmaf(a[i+0], b[i+0], c[i+0]);
    result[i+1] = fmaf(a[i+1], b[i+1], c[i+1]);
    result[i+2] = fmaf(a[i+2], b[i+2], c[i+2]);
    result[i+3] = fmaf(a[i+3], b[i+3], c[i+3]);
}
// অবশিষ্ট elements
for (; i < n; i++) {
    result[i] = fmaf(a[i], b[i], c[i]);
}
```

### ৫. ডেটা টাইপ নির্বাচন

```c
// ভালো:  float ব্যবহার করুন (FPU native)
float x = 1.5f;  // 'f' সাফিক্স গুরুত্বপূর্ণ! 

// খারাপ:  double (সফটওয়্যারে emulate হবে)
double x = 1.5;  // Cortex-M4 FPU তে ধীর
```

---

## উদাহরণ কোড

### ১. সম্পূর্ণ FPU সেটআপ এবং টেস্ট

```c
#include "stm32f4xx. h"
#include <math.h>

// FPU initialization
void SystemInit_FPU(void) {
    // FPU enable করুন
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
    __DSB();
    __ISB();
    
    // Lazy stacking disable (fastest interrupts)
    FPU->FPCCR &= ~(FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk);
}

// FPU performance test
void FPU_PerformanceTest(void) {
    volatile float a = 1.5f;
    volatile float b = 2.3f;
    volatile float result;
    
    // DWT cycle counter enable
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    // Test addition
    uint32_t start = DWT->CYCCNT;
    result = a + b;
    uint32_t cycles_add = DWT->CYCCNT - start;
    
    // Test multiplication
    start = DWT->CYCCNT;
    result = a * b;
    uint32_t cycles_mul = DWT->CYCCNT - start;
    
    // Test division
    start = DWT->CYCCNT;
    result = a / b;
    uint32_t cycles_div = DWT->CYCCNT - start;
    
    // Test sqrt
    start = DWT->CYCCNT;
    result = sqrtf(a);
    uint32_t cycles_sqrt = DWT->CYCCNT - start;
}
```

### ২. DSP অ্যাপ্লিকেশন:  FIR Filter

```c
#define FILTER_LENGTH 32

typedef struct {
    float coeffs[FILTER_LENGTH];
    float history[FILTER_LENGTH];
    uint32_t index;
} FIR_Filter_t;

void FIR_Init(FIR_Filter_t *fir, const float *coeffs) {
    for (int i = 0; i < FILTER_LENGTH; i++) {
        fir->coeffs[i] = coeffs[i];
        fir->history[i] = 0.0f;
    }
    fir->index = 0;
}

float FIR_Process(FIR_Filter_t *fir, float input) {
    // নতুন sample যোগ করুন
    fir->history[fir->index] = input;
    
    // FIR convolution (FMA ব্যবহার করে অপটিমাইজ)
    float output = 0.0f;
    uint32_t idx = fir->index;
    
    for (int i = 0; i < FILTER_LENGTH; i++) {
        output = fmaf(fir->coeffs[i], fir->history[idx], output);
        idx = (idx == 0) ? (FILTER_LENGTH - 1) : (idx - 1);
    }
    
    // Index আপডেট করুন
    fir->index = (fir->index + 1) % FILTER_LENGTH;
    
    return output;
}
```

### ৩. IMU Data Processing (কোয়াটার্নিয়ন)

```c
typedef struct {
    float w, x, y, z;
} Quaternion_t;

// কোয়াটার্নিয়ন নর্মালাইজেশন
void Quaternion_Normalize(Quaternion_t *q) {
    float norm = sqrtf(q->w*q->w + q->x*q->x + q->y*q->y + q->z*q->z);
    if (norm > 0.0f) {
        float inv_norm = 1.0f / norm;
        q->w *= inv_norm;
        q->x *= inv_norm;
        q->y *= inv_norm;
        q->z *= inv_norm;
    }
}

// কোয়াটার্নিয়ন গুণ
Quaternion_t Quaternion_Multiply(const Quaternion_t *q1, const Quaternion_t *q2) {
    Quaternion_t result;
    result.w = q1->w*q2->w - q1->x*q2->x - q1->y*q2->y - q1->z*q2->z;
    result.x = q1->w*q2->x + q1->x*q2->w + q1->y*q2->z - q1->z*q2->y;
    result.y = q1->w*q2->y - q1->x*q2->z + q1->y*q2->w + q1->z*q2->x;
    result.z = q1->w*q2->z + q1->x*q2->y - q1->y*q2->x + q1->z*q2->w;
    return result;
}

// Euler angles to Quaternion
Quaternion_t Euler_to_Quaternion(float roll, float pitch, float yaw) {
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    
    Quaternion_t q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q. y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;
    
    return q;
}
```

### ৪. PID Controller

```c
typedef struct {
    float Kp, Ki, Kd;        // PID gains
    float integral;          // Integral accumulator
    float prev_error;        // Previous error
    float output_min;        // Output limits
    float output_max;
} PID_Controller_t;

void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd,
              float out_min, float out_max) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->output_min = out_min;
    pid->output_max = out_max;
}

float PID_Update(PID_Controller_t *pid, float setpoint, float measured, float dt) {
    // Error calculation
    float error = setpoint - measured;
    
    // Integral (anti-windup সহ)
    pid->integral = fmaf(error, dt, pid->integral);
    
    // Derivative
    float derivative = (error - pid->prev_error) / dt;
    
    // PID output
    float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
    
    // Output limiting
    if (output > pid->output_max) {
        output = pid->output_max;
        pid->integral -= error * dt;  // Anti-windup
    } else if (output < pid->output_min) {
        output = pid->output_min;
        pid->integral -= error * dt;  // Anti-windup
    }
    
    pid->prev_error = error;
    return output;
}
```

### ৫. FFT Pre-processing (Windowing)

```c
#define FFT_SIZE 256

// Hanning window
void Apply_Hanning_Window(float *data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        float window = 0.5f * (1.0f - cosf(2.0f * M_PI * i / (length - 1)));
        data[i] *= window;
    }
}

// Hamming window
void Apply_Hamming_Window(float *data, uint32_t length) {
    const float alpha = 0.54f;
    const float beta = 0.46f;
    
    for (uint32_t i = 0; i < length; i++) {
        float window = alpha - beta * cosf(2.0f * M_PI * i / (length - 1));
        data[i] *= window;
    }
}
```

---

## ডিবাগিং টিপস

### ১.  FPSCR চেক করা

```c
uint32_t Get_FPU_Exceptions(void) {
    uint32_t fpscr;
    __ASM volatile("VMRS %0, FPSCR" : "=r" (fpscr));
    return fpscr & 0x9F;  // Exception flags (bits 0-4, 7)
}

void Clear_FPU_Exceptions(void) {
    uint32_t fpscr;
    __ASM volatile("VMRS %0, FPSCR" : "=r" (fpscr));
    fpscr &= ~0x9F;  // Clear exception flags
    __ASM volatile("VMSR FPSCR, %0" : :  "r" (fpscr));
}
```

### ২. NaN এবং Infinity চেক

```c
#include <math.h>

bool Is_Valid_Float(float value) {
    return !isnan(value) && !isinf(value);
}

void Sanitize_Float(float *value) {
    if (!Is_Valid_Float(*value)) {
        *value = 0.0f;
    }
}
```

### ৩. Performance Profiling

```c
typedef struct {
    uint32_t min_cycles;
    uint32_t max_cycles;
    uint32_t total_cycles;
    uint32_t count;
} PerfCounter_t;

void PerfCounter_Init(PerfCounter_t *perf) {
    perf->min_cycles = UINT32_MAX;
    perf->max_cycles = 0;
    perf->total_cycles = 0;
    perf->count = 0;
}

void PerfCounter_Start(void) {
    DWT->CYCCNT = 0;
}

void PerfCounter_Stop(PerfCounter_t *perf) {
    uint32_t cycles = DWT->CYCCNT;
    
    if (cycles < perf->min_cycles) perf->min_cycles = cycles;
    if (cycles > perf->max_cycles) perf->max_cycles = cycles;
    perf->total_cycles += cycles;
    perf->count++;
}

float PerfCounter_GetAverage(PerfCounter_t *perf) {
    return (float)perf->total_cycles / perf->count;
}
```

---

## সাধারণ সমস্যা এবং সমাধান

### সমস্যা ১: FPU কাজ করছে না
**কারণ**: FPU সক্রিয় করা হয়নি
**সমাধান**:
```c
SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
__DSB();
__ISB();
```

### সমস্যা ২: ধীর floating-point অপারেশন
**কারণ**:  Soft-float ABI ব্যবহার হচ্ছে
**সমাধান**:  Compiler flags ঠিক করুন: 
```
-mfloat-abi=hard -mfpu=fpv4-sp-d16
```

### সমস্যা ৩: Interrupt latency বেড়ে গেছে
**কারণ**:  Lazy context switching চালু আছে
**সমাধান**:
```c
FPU->FPCCR &= ~(FPU_FPCCR_ASPEN_Msk | FPU_FPCCR_LSPEN_Msk);
```

### সমস্যা ৪: Divide by zero crash
**কারণ**: Exception handling সক্রিয় আছে
**সমাধান**: Exception disable করুন বা handle করুন
```c
uint32_t fpscr;
__ASM volatile("VMRS %0, FPSCR" : "=r" (fpscr));
fpscr &= ~(1 << 9);  // Disable DZE
__ASM volatile("VMSR FPSCR, %0" : : "r" (fpscr));
```

---

## রেফারেন্স এবং রিসোর্স

### অফিশিয়াল ডকুমেন্টেশন
1. **ARM Cortex-M4 Technical Reference Manual**
2. **ARMv7-M Architecture Reference Manual**
3. **ARM Compiler toolchain documentation**

### বই
- "The Definitive Guide to ARM Cortex-M4" - Joseph Yiu
- "Digital Signal Processing Using ARM Cortex-M"

### অনলাইন রিসোর্স
- ARM Developer website
- Keil MDK documentation
- ST Microelectronics Application Notes

---

## সংক্ষিপ্ত সারাংশ

### মূল পয়েন্ট: 
✅ **FPU enable করুন**: `CPACR` রেজিস্টার কনফিগার করুন
✅ **Hard-float ABI ব্যবহার করুন**:  Compiler flags সঠিক রাখুন
✅ **FMA ব্যবহার করুন**: `fmaf()` দ্রুত এবং নির্ভুল
✅ **float ব্যবহার করুন, double নয়**: Cortex-M4 শুধু single-precision সাপোর্ট করে
✅ **Exception handling সাবধানে**: Production এ সাধারণত disable রাখুন
✅ **Profile করুন**: DWT cycle counter ব্যবহার করে performance মাপুন

### Performance টিপস: 
🚀 Lazy stacking disable করুন দ্রুত interrupt এর জন্য
🚀 Loop unrolling করুন
🚀 Data অ্যালাইন করুন (4-byte বা 8-byte)
🚀 Reciprocal ব্যবহার করুন বারবার division এর বদলে
🚀 VLDM/VSTM ব্যবহার করুন একাধিক রেজিস্টার transfer এর জন্য

---

**লেখক নোট**:  এই ডকুমেন্টটি ARM Cortex-M4 FPU এর সম্পূর্ণ ডেভেলপমেন্ট গাইড। আপনার প্রোজেক্টে FPU ব্যবহার করার সময় এই নোটগুলি রেফারেন্স হিসেবে ব্যবহার করতে পারেন।

**শেষ আপডেট**: ২০২৫

---