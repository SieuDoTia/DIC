/*
 *  KhửGauss.c
 *
 *  Phiên bản đầu: 2018.01.18
 *  Phiên bản mới: 2018.01.19
 */


// ==== tìm hàng có phần tử đầu lớn nhất (tuyệt đối) và di chuyển nó nằm trên
//                       |
//                       v
//       | *  *   ... *  *  *  *  * ... *  *  *|| x0 |     | * * |
//       |                ...                  || ...|     | ... |
//       | *  *   ... *  *  *  *  * ... *  *  *|| x  |     | * * |
//   --> | *  *   ... *  X  *  *  * ... *  *  *|| x+1|   = | * * |
//       | *  *   ... *  *  *  *  * ... *  *  *|| x+2|     | * * |
//       |                ...                  || ...|     | ... |
//       | *  *   ... *  *  *  *  * ... *  *  *|| xn |     | * * |

//#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define kSAI  0
#define kDUNG 1

unsigned char khuMaTran( float *maTranChanh, unsigned char soLuongHangChanh, unsigned char soLuongCotChanh,
          float *maTranPhu, unsigned char soLuongHangPhu, unsigned char soLuongCotPhu, unsigned char *mangThuTu );
void tinhNghiem( float *maTranChanh, unsigned char soLuongHangChanh, unsigned char soLuongCotChanh,
               float *maTranPhu, unsigned char soLuongHangPhu, unsigned char soLuongCotPhu, unsigned char *mangThuTu );


int main( int argc, char **argv ) {
   
   // ---- tạo ma trận ví dụ. 
   // 4x4
   float maTranChanh[] =
   { 2.0f, 1.0f, 0.0,  0.0f,
     10.0f, 2.0f, 1.0f, 0.0f,
     4.0f, 5.0f, 1.0f, 0.0f,
     0.0f, 0.0f, 1.0f, 2.0f};

   // 4 x 1
   float maTranPhu[] = {
     1.0f, 2.0f,
     1.0f, 2.0f,
     1.0f, 2.0f,
      1.0f, 2.0f};
   
   unsigned char mangThuTu[128] = {
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
      32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
      48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
      64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
      96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
      112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127
   };

   unsigned char soLuongCotChanh = 4;
   unsigned char soLuongCotPhu = 2;
    unsigned char diaChi = mangThuTu[0]*soLuongCotChanh;
   unsigned char diaChiPhu = mangThuTu[0]*soLuongCotPhu;
    printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1] );
    diaChi = mangThuTu[1]*soLuongCotChanh;
   diaChiPhu = mangThuTu[1]*soLuongCotPhu;
    printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1] );
    diaChi = mangThuTu[2]*soLuongCotChanh;
   diaChiPhu = mangThuTu[2]*soLuongCotPhu;
    printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1]  );
    diaChi = mangThuTu[3]*soLuongCotChanh;
   diaChiPhu = mangThuTu[3]*soLuongCotPhu;
    printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1]  );
    printf( "\n" );
   
   unsigned char coNghiem = khuMaTran( maTranChanh, 4, 4, maTranPhu, 4, 2, mangThuTu );
   printf( "coNghiem: %d\n", coNghiem );
   if( coNghiem ) {
      printf( "%2d  %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", mangThuTu[0], maTranChanh[0], maTranChanh[1], maTranChanh[2], maTranChanh[3], maTranPhu[0], maTranPhu[1] );
      printf( "%2d  %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", mangThuTu[1], maTranChanh[4], maTranChanh[5], maTranChanh[6], maTranChanh[7], maTranPhu[2], maTranPhu[3] );
      printf( "%2d  %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", mangThuTu[2], maTranChanh[8], maTranChanh[9], maTranChanh[10], maTranChanh[11], maTranPhu[4], maTranPhu[5]  );
      printf( "%2d  %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", mangThuTu[3], maTranChanh[12], maTranChanh[13], maTranChanh[14], maTranChanh[15], maTranPhu[6], maTranPhu[7]  );
    printf( "\n" );
      tinhNghiem( maTranChanh, 4, 4, maTranPhu, 4, 2, mangThuTu );
   }

   diaChi = mangThuTu[0]*soLuongCotChanh;
   diaChiPhu = mangThuTu[0]*soLuongCotPhu;
   printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1] );
   diaChi = mangThuTu[1]*soLuongCotChanh;
   diaChiPhu = mangThuTu[1]*soLuongCotPhu;
   printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1] );
   diaChi = mangThuTu[2]*soLuongCotChanh;
   diaChiPhu = mangThuTu[2]*soLuongCotPhu;
   printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1]  );
   diaChi = mangThuTu[3]*soLuongCotChanh;
   diaChiPhu = mangThuTu[3]*soLuongCotPhu;
   printf( "%2d %7.3f %7.3f %7.3f %7.3f  %7.3f %7.3f\n", diaChi, maTranChanh[diaChi], maTranChanh[diaChi+1], maTranChanh[diaChi+2], maTranChanh[diaChi+3], maTranPhu[diaChiPhu], maTranPhu[diaChiPhu+1]  );
   printf( "\n" );

   return 1;
}


// ----- chỉ được xài cho hệ phương tuyến tối đa 128 x 128
unsigned char khuMaTran( float *maTranChanh, unsigned char soLuongHangChanh, unsigned char soLuongCotChanh,
          float *maTranPhu, unsigned char soLuongHangPhu, unsigned char soLuongCotPhu, unsigned char *mangThuTu ) {
   
   
   unsigned char soHangDangDungDeKhu = 0;
   
   while( soHangDangDungDeKhu < soLuongHangChanh - 1) {
   
      float giaTriLonNhat = 0.0f;
      unsigned char chiSoHangLonNhat;
      unsigned char soHang = soHangDangDungDeKhu;

      while( soHang < soLuongHangChanh ) {
         // ---- rút giá trị tuyệt đối của hàng này
         float giaTriTuyetDoi = maTranChanh[mangThuTu[soHang]*soLuongCotChanh + soHangDangDungDeKhu];

         if( giaTriTuyetDoi < 0.0f )
            giaTriTuyetDoi = -giaTriTuyetDoi;

         // ---- so sánh giá trị tuyệt đối với lớn nhất
         if( giaTriTuyetDoi > giaTriLonNhat ) {
            giaTriLonNhat = giaTriTuyetDoi;
            chiSoHangLonNhat = soHang;
         }
         
         soHang++;
      }
      
      // ---- xem có nghiệm
      if( giaTriLonNhat == 0.0f )
         return kSAI;
      
      // ---- nâng cấp mảng thứ tự (trao đổi hàng)
      unsigned char so = mangThuTu[soHangDangDungDeKhu];
      mangThuTu[soHangDangDungDeKhu] = mangThuTu[chiSoHangLonNhat];
      mangThuTu[chiSoHangLonNhat] = so;
      
      // ==== chia toàn bộ hàng bởi giá trị lớn nhất
      unsigned soCot = soHangDangDungDeKhu + 1;
      // ---- mẫu số để chia hàng này

      float mauSo = maTranChanh[mangThuTu[soHangDangDungDeKhu]*soLuongCotChanh + soHangDangDungDeKhu];  // không thể dùng biến giá trị lớn nhất vỉ có lẻ nó âm

      // ---- không cần chi nếu mẫu số = 1.0f
      if( mauSo != 1.0f ) {
         // ---- đặt phân tử đầu bằng 1,0f, là kết qủa chia một mình
         maTranChanh[mangThuTu[soHangDangDungDeKhu]*soLuongCotChanh + soHangDangDungDeKhu] = 1.0f;
         // ---- phần tử đầu để chia
         float *phanTuChia = &(maTranChanh[mangThuTu[soHangDangDungDeKhu]*soLuongCotChanh + soHangDangDungDeKhu + 1]);
         while( soCot < soLuongCotChanh ) {
            *phanTuChia /= mauSo;
            phanTuChia++;
            soCot++;
         }
         
         // ---- chia hàng ma trận phụ
         phanTuChia = &(maTranPhu[mangThuTu[soHangDangDungDeKhu]*soLuongCotPhu]);
         soCot = 0;
         while( soCot < soLuongCotPhu ) {
            *phanTuChia /= mauSo;
            phanTuChia++;
            soCot++;
         }
      }
      
      // ==== khử các phân tử trong hàng ở dưới
      unsigned char soHangKhu = soHangDangDungDeKhu+1;
      
      while( soHangKhu < soLuongHangChanh ) {
         float giaTriNhan = maTranChanh[mangThuTu[soHangKhu]*soLuongCotChanh + soHangDangDungDeKhu];

         // ---- nếu phân tử đầu bằng không, không cần khử hàng này
         if( giaTriNhan != 0.0f ) {
            
            maTranChanh[mangThuTu[soHangKhu]*soLuongCotChanh + soHangDangDungDeKhu] = 0.0f;
            float *phanTuGiaTriGoc = &(maTranChanh[mangThuTu[soHangDangDungDeKhu]*soLuongCotChanh + soHangDangDungDeKhu + 1]);
            float *phanTuDeKhu = &(maTranChanh[mangThuTu[soHangKhu]*soLuongCotChanh + soHangDangDungDeKhu + 1]);
            
            // ---- trừ hàng gốc từ hàng khử
            soCot = soHangDangDungDeKhu+1;
            while( soCot < soLuongCotChanh ) {
               *phanTuDeKhu -= giaTriNhan* *phanTuGiaTriGoc;

               phanTuDeKhu++;
               phanTuGiaTriGoc++;
               soCot++;
            }
            
            // ----
            phanTuGiaTriGoc = &(maTranPhu[mangThuTu[soHangDangDungDeKhu]*soLuongCotPhu]);
            phanTuDeKhu = &(maTranPhu[mangThuTu[soHangKhu]*soLuongCotPhu]);
            soCot = 0;
            while( soCot < soLuongCotPhu ) {
               *phanTuDeKhu -= giaTriNhan* *phanTuGiaTriGoc;
               phanTuDeKhu++;
               phanTuGiaTriGoc++;
               soCot++;
            }
            
         }
         
         soHangKhu++;
      }
      soHangDangDungDeKhu++;

   }

   // ==== chia hàng cuối (thật là tính nghiệm của hàng cuối)
   float mauSo = maTranChanh[mangThuTu[soHangDangDungDeKhu]*soLuongCotChanh + soHangDangDungDeKhu];
   if( mauSo != 0.0f ) {
      maTranChanh[mangThuTu[soHangDangDungDeKhu]*soLuongCotChanh + soHangDangDungDeKhu] = 1.0f;
      
      // ----
      float *phanTuChia = &(maTranPhu[mangThuTu[soHangDangDungDeKhu]*soLuongCotPhu]);
      unsigned char soCot = 0;
      while( soCot < soLuongCotPhu ) {
         *phanTuChia /= mauSo;
         phanTuChia++;
         soCot++;
      }

      return kDUNG;   // có nghiệm
   }
   
   return kSAI;
}

// ----- bỏ nghiệm vào ma trận phụ
void tinhNghiem( float *maTranChanh, unsigned char soLuongHangChanh, unsigned char soLuongCotChanh,
               float *maTranPhu, unsigned char soLuongHangPhu, unsigned char soLuongCotPhu, unsigned char *mangThuTu ) {

   unsigned char soCotPhu = 0;  // số cột phụ cho tính nghiệm

   while( soCotPhu < soLuongCotPhu ) {
      
      // ---- hàng cuối có nghiệm sẵn từ khử ma trận Gauss rổi
      char soHangTinhNghiem = soLuongHangChanh - 2;
      
      while( soHangTinhNghiem > -1 ) {
         // ---- tính trở ngược lại
         unsigned char soCotTinh = soLuongCotChanh - 1;
         float *nghiem = &(maTranPhu[mangThuTu[soHangTinhNghiem]*soLuongCotPhu + soCotPhu]);
         float *phanTuTinhChanh = &(maTranChanh[mangThuTu[soHangTinhNghiem]*soLuongCotChanh + soCotTinh]);

         while( soCotTinh > soHangTinhNghiem ) {
            *nghiem -= *phanTuTinhChanh * maTranPhu[mangThuTu[soCotTinh]*soLuongCotPhu + soCotPhu];
            phanTuTinhChanh--;
            soCotTinh--;
         }
         
         soHangTinhNghiem--;
      }
      soCotPhu++;
   }
   
   
}

