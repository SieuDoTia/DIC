/*
 *  FFT_Mới.c
 *
 *  Phiên bản đầu: 2006.07.10
 *  Phiên bản đầu mới: 2017.12.02  14:00
 */

//#include "FFT.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


/* Số Phức */
typedef struct {
   float t;   // phần số thật
   float a;   // phần số ảo
} SoPhuc;


#define kPI 3.141592654
#define kHAI_PI 6.283185307

SoPhuc *FFT( SoPhuc *tinHieu, unsigned int soLuongMauVat );
SoPhuc *nghich_FFT( SoPhuc *tinHieu, unsigned int soLuongMauVat );

// ==== TIN HIỆU, nữa đầu là thật, nữa cuối là aỏ
int main( int argc, char **agrv ) {
   
   // ---- tạo tin hiểu để thử hàm FFT và hàm nghịch_FFT
   unsigned short beDaiTiHieu = 8;
   SoPhuc *tinHieu = malloc( beDaiTiHieu*sizeof(SoPhuc) );
   unsigned char chiSo = 0;
   while ( chiSo < beDaiTiHieu ) {
      if( chiSo < beDaiTiHieu ) {
         tinHieu[chiSo].t = sinf( kHAI_PI*chiSo/4.0f );
         tinHieu[chiSo].a = 0.0f;
      }

      printf( "%d   %5.3f %5.3f\n", chiSo, tinHieu[chiSo].t, tinHieu[chiSo].a );
      chiSo++;
   }
   printf( "\n" );
   
   // ---- biến hóa
   SoPhuc *tinHieuBienHoa = FFT( tinHieu, beDaiTiHieu );
   
   printf( "tin hiệu biến hóa:\n" );
   chiSo = 0;
   while( chiSo < beDaiTiHieu ) {  // nhân 2 vì số phức
      printf( "%d %5.3f %5.3f\n", chiSo, tinHieuBienHoa[chiSo].t, tinHieuBienHoa[chiSo].a );
      chiSo ++;
   }
  printf( "\n" );
   
   // ---- nghịch biến hóa
   SoPhuc *tinHieuTraoLai = nghich_FFT( tinHieuBienHoa, beDaiTiHieu );

   printf( "tin hiệu trở lại:\n" );
   chiSo = 0;
   while( chiSo < beDaiTiHieu ) {
      printf( "%d  %5.3f %5.3f\n", chiSo, tinHieuTraoLai[chiSo].a/beDaiTiHieu, tinHieuTraoLai[chiSo].t/beDaiTiHieu );
      chiSo ++;
   }

   return 1;
}

// ---- Giải thuật Cooley Tukey
SoPhuc *FFT( SoPhuc *tinHieu, unsigned int soLuongMauVat ) {
   
   // ==== KIỂM TRA VÀ SỬA SỐ LƯƠNG MẪU VẬT ====
   // ---- xem nếu số lượng mẫu vật là số mũ hai chẳng và tìm mũ bao nhiêu
   unsigned char muSoHai = 0;
   unsigned int ketQua = soLuongMauVat >> 1;    // chia 2 trước
   
   while( ketQua > 0 ) {
      muSoHai++;             // cộng lên mũ
      ketQua = ketQua >> 1;  // chia 2
   }

   // ---- nếu số lượng mẫu vật không bằng số mũ 2, bỏ số không đằng sau cho số lượng mẫu vật đủ số mũ 2 tiếp
   if( 1 << muSoHai != soLuongMauVat ) {
      // ---- Báo số lượng mẫu vật trong tin hiệu không phải là số mũ hai
      printf( "số lượng mẫu vật của tin hiệu không chẩng số mũ 2! Đang kèm số không cho chẳng\n" );
      // ---- tăng mũ lên một cho được giỡ toàn tin hiệu
      muSoHai++;
      // ---- giữ số lượng mẫu vật ban đầu cho chép tin hiệu
      unsigned int soLuongMauVatBanDau = soLuongMauVat;
      // ---- số lượng mẫu vật mới sau kèm các số không ở phía sau
      soLuongMauVat = 1 << muSoHai;   // số lượng mẫu vật mới
      
      // ---- kèm số không thêm ở phí sau tin hiệu cho đủ số mũ hai
      SoPhuc *tinHieuMoi = calloc( soLuongMauVat, sizeof(SoPhuc) );
      // ---- chép tin hiệu ban đầu
      unsigned short soMauVat = 0;
      while( soMauVat < soLuongMauVat ) {
         tinHieuMoi[soMauVat].t = tinHieu[soMauVat].t;   // phần thật
         tinHieuMoi[soMauVat].a = tinHieu[soMauVat].a;   // phần ảo
         soMauVat++;
      }
      // ---- dùng tin hiệu mới thay tin hiệu cũ
      tinHieu = tinHieuMoi;
   }
   
   // ==== tạo mảng bit ngược hướng ====
   // ---- sắp xếp lại cho được dùng giải thuật dể hơn
   // Ví dụ: 00010 --> 01000
   //        01101 --> 10110
   unsigned int *mangBitNguoc = calloc( soLuongMauVat, sizeof( unsigned int ) );

   char soBit = muSoHai-1;
   while( soBit > -1) {
      unsigned int matNa = 1 << soBit;
      unsigned int matNaNguoc = 1 << (muSoHai - soBit - 1);
      
      unsigned int soMauVat = 0;
      while( soMauVat < soLuongMauVat ) {
         if( (soMauVat & matNa) != 0 )
            mangBitNguoc[soMauVat] = mangBitNguoc[soMauVat] | matNaNguoc;
         
         soMauVat++;
      }
      soBit--;
   }

   // ==== tính hệ số W(n,N) ====
   // ----chỉ cần tính cho 0 ≤ N/2 và nhân –1 cho > N/2  (N = số lương mẫu vật)
   unsigned short soLuongHeSo = soLuongMauVat >> 1;
   SoPhuc *mangHeSo = calloc( soLuongHeSo, sizeof( SoPhuc ) );
   unsigned int soMauVat = 0;
   while( soMauVat < (soLuongMauVat >> 1) ) {
      float goc = -kHAI_PI*(float)soMauVat/(float)soLuongMauVat;
      mangHeSo[soMauVat].t = cosf( goc );
      mangHeSo[soMauVat].a = sinf( goc );
      soMauVat++;
   }

   // ==== BIẾN HÓA ====

   // khi bắt đầu chia thành các tập
   //   - tập đầu chỉ có 2 phân tử (sốLượngPhầnTửTrongTập)
   //   - số lượng tập là sốLượngMẫuVật/sốLượngPhầnTửTrongTập
   
   // ---- tạo mẳng để tính biến hóa
   SoPhuc *mangBienDoi = calloc( soLuongMauVat, sizeof( SoPhuc ) );  // mảng chứa dỡ liệu đang biến đổi
   SoPhuc *mangBienDoiPhu = calloc( soLuongMauVat, sizeof( SoPhuc ) );  // mảng để nhận kết qủa từ biến đổi

   // ---- dùng mảng nghịch bit để sắp xếp dữ liệu đúng thứ tự để áp dụng gaỉi thuật Cooley Tukey
   unsigned short chiSoTinHieu = 0;
   while ( chiSoTinHieu < soLuongMauVat ) {
      mangBienDoi[chiSoTinHieu].t = tinHieu[mangBitNguoc[chiSoTinHieu]].t;
      mangBienDoi[chiSoTinHieu].a = tinHieu[mangBitNguoc[chiSoTinHieu]].a;
      chiSoTinHieu++;
   }

   unsigned short mucBienHoa = 1;  // mức biến đổi nhanh Fourier
   while( mucBienHoa <= muSoHai ) {
      unsigned short soLuongPhanTuTrongTap = 0x01 << mucBienHoa;
      unsigned short phanNuaSoLuongPhanTuTrongTap = soLuongPhanTuTrongTap >> 1;
      unsigned short soLuongTap = soLuongMauVat/soLuongPhanTuTrongTap;
      
//      printf( "  muc %d   soLuongTap %d  soLuongPhanTuTrongTap %d\n", mucBienHoa, soLuongTap, soLuongPhanTuTrongTap );
      unsigned short soTap = 0;  // số tập trong mức biến đổi nhanh Fourier
      unsigned short chiSoBatDau = 0;  // chỉ số trong mảng cho phần tử đầu trong tập đang tính
      while( soTap < soLuongTap ) {
         unsigned short soPhanTuTrongTap = 0;
         unsigned short soLan = chiSoBatDau;
         unsigned short soHeSo = 0;
         while( soPhanTuTrongTap < phanNuaSoLuongPhanTuTrongTap ) {
            unsigned short chiSoMauVatChang = soPhanTuTrongTap+chiSoBatDau;
            unsigned short chiSoMauVatLe = soPhanTuTrongTap+chiSoBatDau + phanNuaSoLuongPhanTuTrongTap;
//            printf( "      [%d] = x[%d] + x[%d]*(w[%d/%d])\n", soLan, chiSoMauVatChang, chiSoMauVatLe, soHeSo, soLuongMauVat );
            mangBienDoiPhu[soLan].t = mangBienDoi[chiSoMauVatChang].t + mangBienDoi[chiSoMauVatLe].t*mangHeSo[soHeSo].t -
                                         mangBienDoi[chiSoMauVatLe].a*mangHeSo[soHeSo].a;
            mangBienDoiPhu[soLan].a = mangBienDoi[chiSoMauVatChang].a + mangBienDoi[chiSoMauVatLe].t*mangHeSo[soHeSo].a +
                                         mangBienDoi[chiSoMauVatLe].a*mangHeSo[soHeSo].t;
//            printf( "      [%d] = x[%d] - x[%d]*(w[%d/%d])\n", soLan + phanNuaSoLuongPhanTuTrongTap, chiSoMauVatChang, chiSoMauVatLe, soHeSo, soLuongMauVat );
            mangBienDoiPhu[soLan + phanNuaSoLuongPhanTuTrongTap].t = mangBienDoi[chiSoMauVatChang].t - mangBienDoi[chiSoMauVatLe].t*mangHeSo[soHeSo].t +
                                         mangBienDoi[chiSoMauVatLe].a*mangHeSo[soHeSo].a;
            mangBienDoiPhu[soLan + phanNuaSoLuongPhanTuTrongTap].a = mangBienDoi[chiSoMauVatChang].a - mangBienDoi[chiSoMauVatLe].t*mangHeSo[soHeSo].a -
                                         mangBienDoi[chiSoMauVatLe].a*mangHeSo[soHeSo].t;
            soPhanTuTrongTap ++;
            soLan++;
            soHeSo+= soLuongTap;
         }
//         printf( "\n" );
         chiSoBatDau += soLuongPhanTuTrongTap;
         soTap++;
      }
      // ---- trao đổi hai mảng
      SoPhuc *mang = mangBienDoi;
      mangBienDoi = mangBienDoiPhu;
      mangBienDoiPhu = mang;

      mucBienHoa++;
   }

   // ---- thả trí nhớ (nên KHÔNG làm nếu cần biến đổi nhiều dữ liệu)
   free( mangHeSo );
   free( mangBienDoiPhu );

   return mangBienDoi;
}


SoPhuc *nghich_FFT( SoPhuc *tinHieu, unsigned int soLuongMauVat ) {
   
   unsigned int chiSo = 0;
   while ( chiSo < soLuongMauVat ) {
      if( chiSo < soLuongMauVat ) {
         float so = tinHieu[chiSo].t;
         tinHieu[chiSo].t = tinHieu[chiSo].a;
         tinHieu[chiSo].a = so;
      }
      
       chiSo++;
   }
   
   return FFT( tinHieu, soLuongMauVat );
}
