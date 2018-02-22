/*
 *  MatBezierTốiƯu.c
 *
 *  Phiên bản đầu: 2018.01.18
 *  Phiên bản mới: 2018.02.22
 
 Tìm một mặt Bezier tối ưu với điểm dữ liếu
 Nó dùng x ≈ t, y ≈ u (tham số t của cong Bezier), không đúng nhưng dễ làm
 --> QUAN TRỌNG: phải giữ phạm vi 0 ≤ x ≤ 1;  0 ≤ y ≤ 1
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
#include <math.h>

#define kSAI  0
#define kDUNG 1

// ====
typedef struct {
   float x;
   float y;
   float z;
} Diem;

/* Chữ Nhật Float */
typedef struct {
   float trai;
   float phai;
   float duoi;
   float tren;
} ChuNhatFloat;

unsigned char khuMaTran( float *maTranChanh, unsigned short soLuongHangChanh, unsigned short soLuongCotChanh,
          float *maTranPhu, unsigned short soLuongHangPhu, unsigned short soLuongCotPhu, unsigned short *mangThuTu );
void tinhNghiem( float *maTranChanh, unsigned char soLuongHangChanh, unsigned char soLuongCotChanh,
               float *maTranPhu, unsigned char soLuongHangPhu, unsigned char soLuongCotPhu, unsigned short *mangThuTu );

/* tính mặt Bezier */
void tinhMatBezier( Diem *mangDiemDieuKhien, float cach_t, float cach_u );

void ketNoiNgang_32x8( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau );
void ketNoiNgang_8x32( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau );

void ketNoiNgang_16x8_matDuoi( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau );
void ketNoiNgang_8x16_matDuoi( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau );

void ketNoiNgang_16x8_matTren( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau );
void ketNoiNgang_8x16_matTren( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau );


/* chiếu ma trận */
void chieu_maTran( float *maTran, unsigned short beRong, unsigned short beCao );
void chieu_maTranKyTu( float *maTran, unsigned short beRong, unsigned short beCao );

void chepMaTranVaoMaTran( float *maTranChep, unsigned short beRongMaTranChep, unsigned short beCaoMaTranChep,
                         float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich,
                         unsigned short soCotDau, unsigned short soHangDau );

void chepGiaTriVaoMaTran( float giaTri, unsigned short beRongMaTranChep, unsigned short beCaoMaTranChep,
                         float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich,
                         unsigned short soCotDau, unsigned short soHangDau );


void matBezierTuongDuong( float *maTranChanh, float *maTranPhu, Diem *mangDiem, unsigned short soLuongDiem, ChuNhatFloat phamVi );


int main( int argc, char **argv ) {
   
   // ==== dữ liệu

   Diem mangDiem[50];

   // ---- cộng thức ví dụ z(x; y) = x*x + y*y
   unsigned short chiSoMang = 0;
   
   unsigned char soDiemY = 0;

   while( soDiemY < 5 ) {
      unsigned char soDiemX = 0;
      float y = soDiemY*0.25f;
      while( soDiemX < 10 ) {
         float x = soDiemX*0.125f;
         mangDiem[chiSoMang].x = x;
         mangDiem[chiSoMang].y = y;
         mangDiem[chiSoMang].z = 0.0;//2.0f - (x*x + y*y);
         printf( "%d %5.3f; %5.3f; %5.3f\n", chiSoMang, x, y, mangDiem[chiSoMang].z );
         soDiemX++;
         chiSoMang++;
      }
      
      soDiemY++;
   }
   
   unsigned short soLuongDiem = chiSoMang;

   unsigned char soLuongHangMat = 1;
   unsigned char soLuongCotMat = 2;
   
   // ---- tạo ma trận chứa hết mặt
   unsigned short beRongMaTranGom = (soLuongCotMat*soLuongHangMat << 5) - ((soLuongCotMat+soLuongHangMat) << 3);
   unsigned short beRongLoiMaTranGom = soLuongCotMat*soLuongHangMat << 4;
   printf( "beRongMaTranGom %d   beRongLoiMaTranGom %d\n", beRongMaTranGom, beRongLoiMaTranGom );

   float *maTranGomChanh = calloc( sizeof( float ), beRongMaTranGom*beRongMaTranGom );
   float *maTranGomPhu = calloc( sizeof( float ), beRongMaTranGom*3 );
//   float *maTranLoiGom = calloc( sizeof( float ), beRongLoiMaTranGom*beRongLoiMaTranGom );

   // ---- tính phạm vi
   ChuNhatFloat phamVi;
   float buocPhamVi_x = 1.0f/soLuongCotMat;
   float buocPhamVi_y = 1.0f/soLuongHangMat;
   
   phamVi.duoi = 0.0f;
   phamVi.tren = buocPhamVi_y;

   unsigned short soMat = 0;
   unsigned short soHangMat = 0;
   while( soHangMat < soLuongHangMat ) {
      phamVi.trai = 0.0f;
      phamVi.phai = buocPhamVi_x;

      unsigned short soCotMat = 0;
      while( soCotMat < soLuongCotMat ) {
         
         // ---- tạo ma trận cho mỗi mặt
         float maTranChanhMat[256];  // 16 x 16
         float maTranPhuMat[48];     // 16 x 3
         
         printf( " phamVi %5.3f %5.3f %5.3f %5.3f\n", phamVi.trai, phamVi.phai, phamVi.duoi, phamVi.tren );
         
         matBezierTuongDuong( maTranChanhMat, maTranPhuMat, mangDiem, soLuongDiem, phamVi );
         
         // ---- chép vào ma trận chánh
         chepMaTranVaoMaTran( maTranChanhMat, 16, 16, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, soMat << 4, soMat << 4 );
         
         // ---- chép vào ma trận phụ
         chepMaTranVaoMaTran( maTranPhuMat, 3, 16, maTranGomPhu, 3, beRongMaTranGom, 0, soMat << 4 );
         
         // ------------
         chieu_maTran( maTranChanhMat, 16, 16 );
         chieu_maTran( maTranPhuMat, 3, 16 );

         soMat++;
         phamVi.trai += buocPhamVi_x;
         phamVi.phai += buocPhamVi_x;
         soCotMat++;
      }
      
      // ==== kết nối
      unsigned short soCotMaTran = (soHangMat*soLuongCotMat << 4);
      unsigned short soHangMaTran = beRongLoiMaTranGom + (((soLuongCotMat << 1) - 1) << 3)*soHangMat;
      
      // ---- kết nối giữa mặt ngang
      unsigned short cot = soCotMaTran;
      unsigned short hang = soHangMaTran;
   
      soCotMat = 1;
      while( soCotMat < soLuongCotMat ) {
         // ---- chép ma trận kết nối ngang 8 x 32
         // viTri cot; hang
     //    printf( " ngang 32 x 8  (%d;%d)\n", cot, hang);
     //    chepGiaTriVaoMaTran( 'o', 32, 8, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, cot, hang );
         ketNoiNgang_32x8( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, cot, hang );

         
         // ---- chép ma trận kết nối ngang 32 x 8
         // viTri hang; cot
//         printf( " ngang 8 x 32  (%d;%d)\n", hang, cot);
//         chepGiaTriVaoMaTran( 'o', 8, 32, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, hang, cot );
         ketNoiNgang_8x32( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, hang, cot );
         cot += 16;
         hang += 8;
         soCotMat++;
      }
      
      // ---- kết nối giữa mặt dộc
      if( soHangMat < soLuongHangMat - 1 ) {
         unsigned short cotChoMatDuoi = soCotMaTran;
         unsigned short hangChoMat = soHangMaTran + ((soLuongCotMat - 1) << 3);
         
         unsigned short cotChoMatTren = soCotMaTran + (soLuongCotMat << 4);
         
         soCotMat = 0;
         while( soCotMat < soLuongCotMat ) {
            // ---- chép ma trận kết nối dộc 8 x 16 mặt dưới
            // viTri cotDuoi; hang
 //           printf( " doc 16 x 8 duoi  (%d;%d)\n", cotChoMatDuoi, hangChoMat);
 //           chepGiaTriVaoMaTran( 'x', 16, 8, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, cotChoMatDuoi, hangChoMat );
            ketNoiNgang_16x8_matDuoi( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, cotChoMatDuoi, hangChoMat );


            // ---- chép ma trận kết nối dộc 8 x 16 mặt trên
            // viTri cotTren; hang
//            printf( " doc 16 x 8 tren (%d;%d)\n", cotChoMatTren, hangChoMat);
//            chepGiaTriVaoMaTran( 'x', 16, 8, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, cotChoMatTren, hangChoMat );
            ketNoiNgang_16x8_matTren( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, cotChoMatTren, hangChoMat );
            
            // ---- chép ma trận kết nối dộc 16 x 8 mặt dưới
            // viTri hang; cotDuoi
//            printf( " doc 8 x 16 duoi  (%d;%d)\n", hangChoMat, cotChoMatDuoi );
//            chepGiaTriVaoMaTran( 'x', 8, 16, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, hangChoMat, cotChoMatDuoi );
            ketNoiNgang_8x16_matDuoi( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, hangChoMat, cotChoMatDuoi );

            // ---- chép ma trận kết nối dộc 16 x 8 mặt trên
            // viTri hang; cotTren
//            printf( " doc 8 x 16 tren (%d;%d)\n", hangChoMat, cotChoMatTren );
//            chepGiaTriVaoMaTran( 'x', 8, 16, maTranGomChanh, beRongMaTranGom, beRongMaTranGom, hangChoMat, cotChoMatTren );
            ketNoiNgang_8x16_matTren( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, hangChoMat, cotChoMatTren );
            
            hangChoMat += 8;
            cotChoMatDuoi += 16;
            cotChoMatTren += 16;
            
            soCotMat++;
         }
      }
      
      // ====
      
      phamVi.duoi += buocPhamVi_y;
      phamVi.tren += buocPhamVi_y;
      soHangMat++;
   }
   
   chieu_maTran( maTranGomChanh, beRongMaTranGom, beRongMaTranGom );
   chieu_maTran( maTranGomPhu, 3, beRongMaTranGom );
   
   unsigned short mangThuTu[256];
   unsigned short chiSo = 0;
   while( chiSo < 256 ) {
      mangThuTu[chiSo] = chiSo;
      chiSo++;
   }

   unsigned char coNghiem = khuMaTran( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, maTranGomPhu, beRongMaTranGom, 3, mangThuTu );
   printf( "coNghiem: %d\n", coNghiem );
   chieu_maTran( maTranGomChanh, beRongMaTranGom, beRongMaTranGom );
   
   if( coNghiem ) {
      tinhNghiem( maTranGomChanh, beRongMaTranGom, beRongMaTranGom, maTranGomPhu, beRongMaTranGom, 3, mangThuTu );
    
      
      Diem mangDiemDieuKhien[16];

      unsigned short chiSo = 0;
      while( chiSo < beRongMaTranGom ) {
         unsigned int chiSoNghiem = mangThuTu[chiSo]*3;
         mangDiemDieuKhien[chiSo].x = maTranGomPhu[chiSoNghiem];
         mangDiemDieuKhien[chiSo].y = maTranGomPhu[chiSoNghiem+1];
         mangDiemDieuKhien[chiSo].z = maTranGomPhu[chiSoNghiem+2];
         printf( "%d %5.3f %5.3f %5.3f\n", chiSo, mangDiemDieuKhien[chiSo].x, mangDiemDieuKhien[chiSo].y, mangDiemDieuKhien[chiSo].z );
         chiSo++;
      }
      
      // ---- tính mặt bezier
      tinhMatBezier( mangDiemDieuKhien, 0.25f, 0.25f );

   }

   free( maTranGomChanh );
   free( maTranGomPhu );
   return 1;
}


// ----- chỉ được xài cho hệ phương tuyến
unsigned char khuMaTran( float *maTranChanh, unsigned short soLuongHangChanh, unsigned short soLuongCotChanh,
          float *maTranPhu, unsigned short soLuongHangPhu, unsigned short soLuongCotPhu, unsigned short *mangThuTu ) {
   
   unsigned short soHangDangDungDeKhu = 0;
   
   while( soHangDangDungDeKhu < soLuongHangChanh - 1) {
   
      float giaTriLonNhat = 0.0f;
      unsigned short chiSoHangLonNhat;
      unsigned short soHang = soHangDangDungDeKhu;

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
      
//      printf(" mangThuTu  doi soHangDangDungDeKhu %d voi chiSoHangLonNhat %d\n", soHangDangDungDeKhu, chiSoHangLonNhat );
//      printf("           doi mangThutu[soHangDangDungDeKhu] %d voi mangThuTu[chiSoHangLonNhat] %d\n", mangThuTu[soHangDangDungDeKhu], mangThuTu[chiSoHangLonNhat] );
      // ---- nâng cấp mảng thứ tự (trao đổi hàng)
      unsigned short so = mangThuTu[soHangDangDungDeKhu];
      mangThuTu[soHangDangDungDeKhu] = mangThuTu[chiSoHangLonNhat];
      mangThuTu[chiSoHangLonNhat] = so;
      
//      printf(" mangThuTu\n" );
//      printf("  %d\n", mangThuTu[0]);
//      printf("  %d\n", mangThuTu[1]);
//      printf("  %d\n", mangThuTu[2]);
//      printf("  %d\n", mangThuTu[3]);
   
      // ==== chia toàn bộ hàng bởi giá trị lớn nhất
      unsigned short soCot = soHangDangDungDeKhu + 1;
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
      unsigned short soHangKhu = soHangDangDungDeKhu+1;
      
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
      unsigned short soCot = 0;
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
               float *maTranPhu, unsigned char soLuongHangPhu, unsigned char soLuongCotPhu, unsigned short *mangThuTu ) {

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


void matBezierTuongDuong( float *maTranChanh, float *maTranPhu, Diem *mangDiem, unsigned short soLuongDiem, ChuNhatFloat phamVi ) {

   // ---- phạm vi
   float beRongPhamViX = phamVi.phai - phamVi.trai;
   float beRongPhamViY = phamVi.tren - phamVi.duoi;
   if( beRongPhamViX == 0.0f || beRongPhamViY == 0.0f ) {
      printf( "matBezierTuongDuong: SAI LẦM: phạm vi == 0.0\n" );
      exit(0);
   }
   else if( beRongPhamViX < 0.0f ) {
      beRongPhamViX = -beRongPhamViX;
      float so = phamVi.trai;
      phamVi.trai = phamVi.phai;
      phamVi.phai = so;
   }
   
   if( beRongPhamViY < 0.0f ) {
      beRongPhamViY = -beRongPhamViY;
      float so = phamVi.duoi;
      phamVi.duoi = phamVi.tren;
      phamVi.tren = so;
   }

   // ---- xóa ma ma trận
   unsigned short chiSoMaTran = 0;
   while( chiSoMaTran < 256 ) {
      maTranChanh[chiSoMaTran] = 0.0f;
      chiSoMaTran++;
   }

   chiSoMaTran = 0;
   while( chiSoMaTran < 48 ) {
      maTranPhu[chiSoMaTran] = 0.0f;
      chiSoMaTran++;
   }

   // ---- đặt giá trị trong ma trận
   unsigned short chiSoDiem = 0;
   while( chiSoDiem < soLuongDiem ) {
      
      // ---- chỉ gồm điểm trong phạm vi
      float x = mangDiem[chiSoDiem].x;
      float y = mangDiem[chiSoDiem].y;
      float z = mangDiem[chiSoDiem].z;
      
      unsigned char trongPhamVi = kDUNG;
      if( x < phamVi.trai )
         trongPhamVi = kSAI;
      else if( x > phamVi.phai )
         trongPhamVi = kSAI;
      else if( y < phamVi.duoi )
         trongPhamVi = kSAI;
      else if( y > phamVi.tren )
         trongPhamVi = kSAI;
      
      if( trongPhamVi ) {
         // ---- tính ma trận
         float t = (x - phamVi.trai)/beRongPhamViX;
         float nghich_t = 1.0f - t;
         
         float u = (y - phamVi.duoi)/beRongPhamViY;
         float nghich_u = 1.0f - u;
         printf( "trong pham vi  %5.3f %5.3f %5.3f   t %5.3f  u %5.3f\n", x, y, z, t, u );

         float t_60 = pow( nghich_t, 6.0f );
         float t_51 = pow( nghich_t, 5.0f )*t;
         float t_42 = pow( nghich_t, 4.0f )*t*t;
         float t_33 = pow( nghich_t, 3.0f )*t*t*t;
         float t_24 = nghich_t*nghich_t * t*t*t*t;
         float t_15 = nghich_t * t*t*t*t*t;
         float t_06 = t*t*t*t*t*t;
         
         float u_60 = pow( nghich_u, 6.0f );
         float u_51 = pow( nghich_u, 5.0f )*u;
         float u_42 = pow( nghich_u, 4.0f )*u*u;
         float u_33 = pow( nghich_u, 3.0f )*u*u*u;
         float u_24 = nghich_u*nghich_u * u*u*u*u;
         float u_15 = nghich_u * u*u*u*u*u;
         float u_06 = u*u*u*u*u*u;
         
         maTranChanh[0] += t_60*u_60;
         maTranChanh[1] += 3.0f*t_60*u_51;
         maTranChanh[2] += 3.0f*t_60*u_42;
         maTranChanh[3] += t_60*u_33;
         
         maTranChanh[4] += 3.0f*t_51*u_60;
         maTranChanh[5] += 9.0f*t_51*u_51;
         maTranChanh[6] += 9.0f*t_51*u_42;
         maTranChanh[7] += 3.0f*t_51*u_33;
         
         maTranChanh[8] += 3.0f*t_42*u_60;
         maTranChanh[9] += 9.0f*t_42*u_51;
         maTranChanh[10] += 9.0f*t_42*u_42;
         maTranChanh[11] += 3.0f*t_42*u_33;
         
         maTranChanh[12] += t_33*u_60;
         maTranChanh[13] += 3.0f*t_33*u_51;
         maTranChanh[14] += 3.0f*t_33*u_42;
         maTranChanh[15] += t_33*u_33;
         
         // ----
         maTranChanh[16] += t_60*u_51;
         maTranChanh[17] += 3.0f*t_60*u_42;
         maTranChanh[18] += 3.0f*t_60*u_33;
         maTranChanh[19] += t_60*u_24;
         
         maTranChanh[20] += 3.0f*t_51*u_51;
         maTranChanh[21] += 9.0f*t_51*u_42;
         maTranChanh[22] += 9.0f*t_51*u_33;
         maTranChanh[23] += 3.0f*t_51*u_24;
         
         maTranChanh[24] += 3.0f*t_42*u_51;
         maTranChanh[25] += 9.0f*t_42*u_42;
         maTranChanh[26] += 9.0f*t_42*u_33;
         maTranChanh[27] += 3.0f*t_42*u_24;
         
         maTranChanh[28] += t_33*u_51;
         maTranChanh[29] += 3.0f*t_33*u_42;
         maTranChanh[30] += 3.0f*t_33*u_33;
         maTranChanh[31] += t_33*u_24;
         
         // ----
         maTranChanh[32] += t_60*u_42;
         maTranChanh[33] += 3.0f*t_60*u_33;
         maTranChanh[34] += 3.0f*t_60*u_24;
         maTranChanh[35] += t_60*u_15;
         
         maTranChanh[36] += 3.0f*t_51*u_42;
         maTranChanh[37] += 9.0f*t_51*u_33;
         maTranChanh[38] += 9.0f*t_51*u_24;
         maTranChanh[39] += 3.0f*t_51*u_15;
         
         maTranChanh[40] += 3.0f*t_42*u_42;
         maTranChanh[41] += 9.0f*t_42*u_33;
         maTranChanh[42] += 9.0f*t_42*u_24;
         maTranChanh[43] += 3.0f*t_42*u_15;
         
         maTranChanh[44] += t_33*u_42;
         maTranChanh[45] += 3.0f*t_33*u_33;
         maTranChanh[46] += 3.0f*t_33*u_24;
         maTranChanh[47] += t_33*u_15;
         
         // ----
         maTranChanh[48] += t_60*u_33;
         maTranChanh[49] += 3.0f*t_60*u_24;
         maTranChanh[50] += 3.0f*t_60*u_15;
         maTranChanh[51] += t_60*u_06;
         
         maTranChanh[52] += 3.0f*t_51*u_33;
         maTranChanh[53] += 9.0f*t_51*u_24;
         maTranChanh[54] += 9.0f*t_51*u_15;
         maTranChanh[55] += 3.0f*t_51*u_06;
         
         maTranChanh[56] += 3.0f*t_42*u_33;
         maTranChanh[57] += 9.0f*t_42*u_24;
         maTranChanh[58] += 9.0f*t_42*u_15;
         maTranChanh[59] += 3.0f*t_42*u_06;
         
         maTranChanh[60] += t_33*u_33;
         maTranChanh[61] += 3.0f*t_33*u_24;
         maTranChanh[62] += 3.0f*t_33*u_15;
         maTranChanh[63] += t_33*u_06;
         //      printf( "%d %5.3f %5.3f %5.3f %5.3f\n", chiSoDiem, maTranChanh[0], maTranChanh[1], maTranChanh[2], maTranChanh[3] );
         // ====
         maTranChanh[64] += t_51*u_60;
         maTranChanh[65] += 3.0f*t_51*u_51;
         maTranChanh[66] += 3.0f*t_51*u_42;
         maTranChanh[67] += t_51*u_33;
         
         maTranChanh[68] += 3.0f*t_42*u_60;
         maTranChanh[69] += 9.0f*t_42*u_51;
         maTranChanh[70] += 9.0f*t_42*u_42;
         maTranChanh[71] += 3.0f*t_42*u_33;
         
         maTranChanh[72] += 3.0f*t_33*u_60;
         maTranChanh[73] += 9.0f*t_33*u_51;
         maTranChanh[74] += 9.0f*t_33*u_42;
         maTranChanh[75] += 3.0f*t_33*u_33;
         
         maTranChanh[76] += t_24*u_60;
         maTranChanh[77] += 3.0f*t_24*u_51;
         maTranChanh[78] += 3.0f*t_24*u_42;
         maTranChanh[79] += t_24*u_33;
         
         // ----
         maTranChanh[80] += t_51*u_51;
         maTranChanh[81] += 3.0f*t_51*u_42;
         maTranChanh[82] += 3.0f*t_51*u_33;
         maTranChanh[83] += t_51*u_24;
         
         maTranChanh[84] += 3.0f*t_42*u_51;
         maTranChanh[85] += 9.0f*t_42*u_42;
         maTranChanh[86] += 9.0f*t_42*u_33;
         maTranChanh[87] += 3.0f*t_42*u_24;
         
         maTranChanh[88] += 3.0f*t_33*u_51;
         maTranChanh[89] += 9.0f*t_33*u_42;
         maTranChanh[90] += 9.0f*t_33*u_33;
         maTranChanh[91] += 3.0f*t_33*u_24;
         
         maTranChanh[92] += t_24*u_51;
         maTranChanh[93] += 3.0f*t_24*u_42;
         maTranChanh[94] += 3.0f*t_24*u_33;
         maTranChanh[95] += t_24*u_24;
         
         // ----
         maTranChanh[96] += t_51*u_42;
         maTranChanh[97] += 3.0f*t_51*u_33;
         maTranChanh[98] += 3.0f*t_51*u_24;
         maTranChanh[99] += t_51*u_15;
         
         maTranChanh[100] += 3.0f*t_42*u_42;
         maTranChanh[101] += 9.0f*t_42*u_33;
         maTranChanh[102] += 9.0f*t_42*u_24;
         maTranChanh[103] += 3.0f*t_42*u_15;
         
         maTranChanh[104] += 3.0f*t_33*u_42;
         maTranChanh[105] += 9.0f*t_33*u_33;
         maTranChanh[106] += 9.0f*t_33*u_24;
         maTranChanh[107] += 3.0f*t_33*u_15;
         
         maTranChanh[108] += t_24*u_42;
         maTranChanh[109] += 3.0f*t_24*u_33;
         maTranChanh[110] += 3.0f*t_24*u_24;
         maTranChanh[111] += t_24*u_15;
         
         // ----
         maTranChanh[112] += t_51*u_33;
         maTranChanh[113] += 3.0f*t_51*u_24;
         maTranChanh[114] += 3.0f*t_51*u_15;
         maTranChanh[115] += t_51*u_06;
         
         maTranChanh[116] += 3.0f*t_42*u_33;
         maTranChanh[117] += 9.0f*t_42*u_24;
         maTranChanh[118] += 9.0f*t_42*u_15;
         maTranChanh[119] += 3.0f*t_42*u_06;
         
         maTranChanh[120] += 3.0f*t_33*u_33;
         maTranChanh[121] += 9.0f*t_33*u_24;
         maTranChanh[122] += 9.0f*t_33*u_15;
         maTranChanh[123] += 3.0f*t_33*u_06;
         
         maTranChanh[124] += t_24*u_33;
         maTranChanh[125] += 3.0f*t_24*u_24;
         maTranChanh[126] += 3.0f*t_24*u_15;
         maTranChanh[127] += t_24*u_06;
         
         // ====
         maTranChanh[128] += t_42*u_60;
         maTranChanh[129] += 3.0f*t_42*u_51;
         maTranChanh[130] += 3.0f*t_42*u_42;
         maTranChanh[131] += t_42*u_33;
         
         maTranChanh[132] += 3.0f*t_33*u_60;
         maTranChanh[133] += 9.0f*t_33*u_51;
         maTranChanh[134] += 9.0f*t_33*u_42;
         maTranChanh[135] += 3.0f*t_33*u_33;
         
         maTranChanh[136] += 3.0f*t_24*u_60;
         maTranChanh[137] += 9.0f*t_24*u_51;
         maTranChanh[138] += 9.0f*t_24*u_42;
         maTranChanh[139] += 3.0f*t_24*u_33;
         
         maTranChanh[140] += t_15*u_60;
         maTranChanh[141] += 3.0f*t_15*u_51;
         maTranChanh[142] += 3.0f*t_15*u_42;
         maTranChanh[143] += t_15*u_33;
         
         // ----
         maTranChanh[144] += t_42*u_51;
         maTranChanh[145] += 3.0f*t_42*u_42;
         maTranChanh[146] += 3.0f*t_42*u_33;
         maTranChanh[147] += t_42*u_24;
         
         maTranChanh[148] += 3.0f*t_33*u_51;
         maTranChanh[149] += 9.0f*t_33*u_42;
         maTranChanh[150] += 9.0f*t_33*u_33;
         maTranChanh[151] += 3.0f*t_33*u_24;
         
         maTranChanh[152] += 3.0f*t_24*u_51;
         maTranChanh[153] += 9.0f*t_24*u_42;
         maTranChanh[154] += 9.0f*t_24*u_33;
         maTranChanh[155] += 3.0f*t_24*u_24;
         
         maTranChanh[156] += t_15*u_51;
         maTranChanh[157] += 3.0f*t_15*u_42;
         maTranChanh[158] += 3.0f*t_15*u_33;
         maTranChanh[159] += t_15*u_24;
         
         // ----
         maTranChanh[160] += t_42*u_42;
         maTranChanh[161] += 3.0f*t_42*u_33;
         maTranChanh[162] += 3.0f*t_42*u_24;
         maTranChanh[163] += t_42*u_15;
         
         maTranChanh[164] += 3.0f*t_33*u_42;
         maTranChanh[165] += 9.0f*t_33*u_33;
         maTranChanh[166] += 9.0f*t_33*u_24;
         maTranChanh[167] += 3.0f*t_33*u_15;
         
         maTranChanh[168] += 3.0f*t_24*u_42;
         maTranChanh[169] += 9.0f*t_24*u_33;
         maTranChanh[170] += 9.0f*t_24*u_24;
         maTranChanh[171] += 3.0f*t_24*u_15;
         
         maTranChanh[172] += t_15*u_42;
         maTranChanh[173] += 3.0f*t_15*u_33;
         maTranChanh[174] += 3.0f*t_15*u_24;
         maTranChanh[175] += t_15*u_15;
         
         // ----
         maTranChanh[176] += t_42*u_33;
         maTranChanh[177] += 3.0f*t_42*u_24;
         maTranChanh[178] += 3.0f*t_42*u_15;
         maTranChanh[179] += t_42*u_06;
         
         maTranChanh[180] += 3.0f*t_33*u_33;
         maTranChanh[181] += 9.0f*t_33*u_24;
         maTranChanh[182] += 9.0f*t_33*u_15;
         maTranChanh[183] += 3.0f*t_33*u_06;
         
         maTranChanh[184] += 3.0f*t_24*u_33;
         maTranChanh[185] += 9.0f*t_24*u_24;
         maTranChanh[186] += 9.0f*t_24*u_15;
         maTranChanh[187] += 3.0f*t_24*u_06;
         
         maTranChanh[188] += t_15*u_33;
         maTranChanh[189] += 3.0f*t_15*u_24;
         maTranChanh[190] += 3.0f*t_15*u_15;
         maTranChanh[191] += t_15*u_06;
         
         // ====
         maTranChanh[192] += t_33*u_60;
         maTranChanh[193] += 3.0f*t_33*u_51;
         maTranChanh[194] += 3.0f*t_33*u_42;
         maTranChanh[195] += t_33*u_33;
         
         maTranChanh[196] += 3.0f*t_24*u_60;
         maTranChanh[197] += 9.0f*t_24*u_51;
         maTranChanh[198] += 9.0f*t_24*u_42;
         maTranChanh[199] += 3.0f*t_24*u_33;
         
         maTranChanh[200] += 3.0f*t_15*u_60;
         maTranChanh[201] += 9.0f*t_15*u_51;
         maTranChanh[202] += 9.0f*t_15*u_42;
         maTranChanh[203] += 3.0f*t_15*u_33;
         
         maTranChanh[204] += t_06*u_60;
         maTranChanh[205] += 3.0f*t_06*u_51;
         maTranChanh[206] += 3.0f*t_06*u_42;
         maTranChanh[207] += t_06*u_33;
         
         // ----
         maTranChanh[208] += t_33*u_51;
         maTranChanh[209] += 3.0f*t_33*u_42;
         maTranChanh[210] += 3.0f*t_33*u_33;
         maTranChanh[211] += t_33*u_24;
         
         maTranChanh[212] += 3.0f*t_24*u_51;
         maTranChanh[213] += 9.0f*t_24*u_42;
         maTranChanh[214] += 9.0f*t_24*u_33;
         maTranChanh[215] += 3.0f*t_24*u_24;
         
         maTranChanh[216] += 3.0f*t_15*u_51;
         maTranChanh[217] += 9.0f*t_15*u_42;
         maTranChanh[218] += 9.0f*t_15*u_33;
         maTranChanh[219] += 3.0f*t_15*u_24;
         
         maTranChanh[220] += t_06*u_51;
         maTranChanh[221] += 3.0f*t_06*u_42;
         maTranChanh[222] += 3.0f*t_06*u_33;
         maTranChanh[223] += t_06*u_24;
         
         // ----
         maTranChanh[224] += t_33*u_42;
         maTranChanh[225] += 3.0f*t_33*u_33;
         maTranChanh[226] += 3.0f*t_33*u_24;
         maTranChanh[227] += t_33*u_15;
         
         maTranChanh[228] += 3.0f*t_24*u_42;
         maTranChanh[229] += 9.0f*t_24*u_33;
         maTranChanh[230] += 9.0f*t_24*u_24;
         maTranChanh[231] += 3.0f*t_24*u_15;
         
         maTranChanh[232] += 3.0f*t_15*u_42;
         maTranChanh[233] += 9.0f*t_15*u_33;
         maTranChanh[234] += 9.0f*t_15*u_24;
         maTranChanh[235] += 3.0f*t_15*u_15;
         
         maTranChanh[236] += t_06*u_42;
         maTranChanh[237] += 3.0f*t_06*u_33;
         maTranChanh[238] += 3.0f*t_06*u_24;
         maTranChanh[239] += t_06*u_15;
         
         // ----
         maTranChanh[240] += t_33*u_33;
         maTranChanh[241] += 3.0f*t_33*u_24;
         maTranChanh[242] += 3.0f*t_33*u_15;
         maTranChanh[243] += t_33*u_06;
         
         maTranChanh[244] += 3.0f*t_24*u_33;
         maTranChanh[245] += 9.0f*t_24*u_24;
         maTranChanh[246] += 9.0f*t_24*u_15;
         maTranChanh[247] += 3.0f*t_24*u_06;
         
         maTranChanh[248] += 3.0f*t_15*u_33;
         maTranChanh[249] += 9.0f*t_15*u_24;
         maTranChanh[250] += 9.0f*t_15*u_15;
         maTranChanh[251] += 3.0f*t_15*u_06;
         
         maTranChanh[252] += t_06*u_33;
         maTranChanh[253] += 3.0f*t_06*u_24;
         maTranChanh[254] += 3.0f*t_06*u_15;
         maTranChanh[255] += t_06*u_06;
         
         float t_30 = nghich_t*nghich_t*nghich_t;
         float t_21 = nghich_t*nghich_t*t;
         float t_12 = nghich_t*t*t;
         float t_03 = t*t*t;
         
         float u_30 = nghich_u*nghich_u*nghich_u;
         float u_21 = nghich_u*nghich_u*u;
         float u_12 = nghich_u*u*u;
         float u_03 = u*u*u;
         
         // ----
         maTranPhu[0] += x*t_30*u_30;
         maTranPhu[3] += x*t_30*u_21;
         maTranPhu[6] += x*t_30*u_12;
         maTranPhu[9] += x*t_30*u_03;
         
         maTranPhu[12] += x*t_21*u_30;
         maTranPhu[15] += x*t_21*u_21;
         maTranPhu[18] += x*t_21*u_12;
         maTranPhu[21] += x*t_21*u_03;
         
         maTranPhu[24] += x*t_12*u_30;
         maTranPhu[27] += x*t_12*u_21;
         maTranPhu[30] += x*t_12*u_12;
         maTranPhu[33] += x*t_12*u_03;
         
         maTranPhu[36] += x*t_03*u_30;
         maTranPhu[39] += x*t_03*u_21;
         maTranPhu[42] += x*t_03*u_12;
         maTranPhu[45] += x*t_03*u_03;
         
         // ----
         maTranPhu[1] += y*t_30*u_30;
         maTranPhu[4] += y*t_30*u_21;
         maTranPhu[7] += y*t_30*u_12;
         maTranPhu[10] += y*t_30*u_03;
         
         maTranPhu[13] += y*t_21*u_30;
         maTranPhu[16] += y*t_21*u_21;
         maTranPhu[19] += y*t_21*u_12;
         maTranPhu[22] += y*t_21*u_03;
         
         maTranPhu[25] += y*t_12*u_30;
         maTranPhu[28] += y*t_12*u_21;
         maTranPhu[31] += y*t_12*u_12;
         maTranPhu[34] += y*t_12*u_03;
         
         maTranPhu[37] += y*t_03*u_30;
         maTranPhu[40] += y*t_03*u_21;
         maTranPhu[43] += y*t_03*u_12;
         maTranPhu[46] += y*t_03*u_03;
         
         // ----
         maTranPhu[2] += z*t_30*u_30;
         maTranPhu[5] += z*t_30*u_21;
         maTranPhu[8] += z*t_30*u_12;
         maTranPhu[11] += z*t_30*u_03;
         
         maTranPhu[14] += z*t_21*u_30;
         maTranPhu[17] += z*t_21*u_21;
         maTranPhu[20] += z*t_21*u_12;
         maTranPhu[23] += z*t_21*u_03;
         
         maTranPhu[26] += z*t_12*u_30;
         maTranPhu[29] += z*t_12*u_21;
         maTranPhu[32] += z*t_12*u_12;
         maTranPhu[35] += z*t_12*u_03;
         
         maTranPhu[38] += z*t_03*u_30;
         maTranPhu[41] += z*t_03*u_21;
         maTranPhu[44] += z*t_03*u_12;
         maTranPhu[47] += z*t_03*u_03;
      }

      chiSoDiem++;
   }

}



#pragma mark ---- TÍNh MẶT BEZIER
Diem tinhViTriBezier3C( Diem *mangDiemDieuKhien, float thamSo );  // từ chương trình DO TÌA
// Cần 16 điểm điều khiển và cách giữa t và u
// Dùng giải thuật de Casteljau để tính các trên mặt Bezier (ổn định hơn dùng cộng thức Bezier trực tiếp)
//     - Tính 4 điền khiển giữa

//        ^ u
//        |
//        |
//  (0;1) +----+-----+----+ (1;1)
//        |               |
//        +    +     +    +
//        |               |
//        +    +     +    +
//        |               |
//  (0;0) +----+-----+----+ (1;0)  ---> t

void tinhMatBezier( Diem *mangDiemDieuKhien, float cach_t, float cach_u ) {

   float u = 0.0f;
   
   while( u <= 1.0f ) {
      
      // ---- tính 4 điểm điều khiển giữa cho u
      Diem diemDieuKhien_giua[4];
      diemDieuKhien_giua[0] = tinhViTriBezier3C( &(mangDiemDieuKhien[0]), u );
      diemDieuKhien_giua[1] = tinhViTriBezier3C( &(mangDiemDieuKhien[4]), u );
      diemDieuKhien_giua[2] = tinhViTriBezier3C( &(mangDiemDieuKhien[8]), u );
      diemDieuKhien_giua[3] = tinhViTriBezier3C( &(mangDiemDieuKhien[12]), u );

      float t = 0.0f;
      while( t <= 1.0f ) {
         Diem diemTrenMat = tinhViTriBezier3C( diemDieuKhien_giua, t );
         printf("(%5.3f; %5.3f)  (%5.3f; %5.3f; %5.3f) \n", t, u, diemTrenMat.x, diemTrenMat.y ,diemTrenMat.z );
         t += cach_t;
      }
      u += cach_u;
      printf("\n" );
   }
}


#pragma mark ---- Bezier
// từ chương trình DO TÌA
Diem tinhViTriBezier3C( Diem *mangDiemDieuKhien, float thamSo ) {
   
   // ---- mức 0, cần tính 3 điểm
   Diem muc0[3];
   muc0[0].x = (1.0f - thamSo)*mangDiemDieuKhien[0].x + thamSo*mangDiemDieuKhien[1].x;
   muc0[0].y = (1.0f - thamSo)*mangDiemDieuKhien[0].y + thamSo*mangDiemDieuKhien[1].y;
   muc0[0].z = (1.0f - thamSo)*mangDiemDieuKhien[0].z + thamSo*mangDiemDieuKhien[1].z;
   muc0[1].x = (1.0f - thamSo)*mangDiemDieuKhien[1].x + thamSo*mangDiemDieuKhien[2].x;
   muc0[1].y = (1.0f - thamSo)*mangDiemDieuKhien[1].y + thamSo*mangDiemDieuKhien[2].y;
   muc0[1].z = (1.0f - thamSo)*mangDiemDieuKhien[1].z + thamSo*mangDiemDieuKhien[2].z;
   muc0[2].x = (1.0f - thamSo)*mangDiemDieuKhien[2].x + thamSo*mangDiemDieuKhien[3].x;
   muc0[2].y = (1.0f - thamSo)*mangDiemDieuKhien[2].y + thamSo*mangDiemDieuKhien[3].y;
   muc0[2].z = (1.0f - thamSo)*mangDiemDieuKhien[2].z + thamSo*mangDiemDieuKhien[3].z;
   
   // ---- mức 1, cần tính 2 điểm
   Diem muc1[2];
   muc1[0].x = (1.0f - thamSo)*muc0[0].x + thamSo*muc0[1].x;
   muc1[0].y = (1.0f - thamSo)*muc0[0].y + thamSo*muc0[1].y;
   muc1[0].z = (1.0f - thamSo)*muc0[0].z + thamSo*muc0[1].z;
   muc1[1].x = (1.0f - thamSo)*muc0[1].x + thamSo*muc0[2].x;
   muc1[1].y = (1.0f - thamSo)*muc0[1].y + thamSo*muc0[2].y;
   muc1[1].z = (1.0f - thamSo)*muc0[1].z + thamSo*muc0[2].z;
   
   Diem viTri;
   viTri.x = (1.0f - thamSo)*muc1[0].x + thamSo*muc1[1].x;
   viTri.y = (1.0f - thamSo)*muc1[0].y + thamSo*muc1[1].y;
   viTri.z = (1.0f - thamSo)*muc1[0].z + thamSo*muc1[1].z;
   
   return viTri;
}

#pragma mark ---- Chép Ma Trận
// chép toàn bộ ma trận chép vào ma trận địch tại số cột và số hàng đã chọn
void chepMaTranVaoMaTran( float *maTranChep, unsigned short beRongMaTranChep, unsigned short beCaoMaTranChep,
                         float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich,
                         unsigned short soCotDau, unsigned short soHangDau ) {
   
   // ---- nếu số cột và hàng bắt đầu hơn bề rộng và bề cao ma trận đích, không cần chép gì cả
   if( maTranChep == NULL ) {
      printf( "chepMaTranVaoMaTran: maTranChep == NULL\n" );
      return;
   }
   if( maTranDich == NULL ) {
      printf( "chepMaTranVaoMaTran: maTranDich == NULL\n" );
      return;
   }
   
   // ---- chỉ chép được nếu ma trận chép ở trong phạm vi ma trận đích
   if( (soCotDau < beRongMaTranDich) && (soHangDau < beCaoMaTranDich) ) {
      
      // ---- tính số cột và số hàng cuối
      unsigned short soCotCuoi = soCotDau + beRongMaTranChep;
      unsigned short soHangCuoi = soHangDau + beCaoMaTranChep;
      if( soCotCuoi > beRongMaTranDich )
         soCotCuoi = beRongMaTranDich;
      
      if( soHangCuoi > beCaoMaTranDich )
         soHangCuoi = beCaoMaTranDich;
      
      // ---- chép giá trị phân tử ma trận chép vào ma trận đích
      unsigned short soHang = soHangDau;
      unsigned short soHangMaTranChep = 0;
      
      while( soHang < soHangCuoi  ) {
         
         unsigned short soCot = soCotDau;
         unsigned int diaChiMaTranDich = soHang*beRongMaTranDich + soCot;
         unsigned int diaChiMaTranChep = soHangMaTranChep*beRongMaTranChep;
         while( soCot < soCotCuoi ) {
            maTranDich[diaChiMaTranDich] = maTranChep[diaChiMaTranChep];
            diaChiMaTranDich++;
            diaChiMaTranChep++;
            soCot++;
         }
         soHangMaTranChep++;
         soHang++;
      }
   }
}

#pragma mark ---- Kết Nối Ngang
// cùng điểm
// 0  0  0  1   0  0  0  0   0  0  0  0   0  0  0  0  -1  0  0  0   0  0  0  0   0  0  0  0   0  0  0  0
// 0  0  0  0   0  0  0  1   0  0  0  0   0  0  0  0   0  0  0  0  -1  0  0  0   0  0  0  0   0  0  0  0
// 0  0  0  0   0  0  0  0   0  0  0  1   0  0  0  0   0  0  0  0   0  0  0  0  -1  0  0  0   0  0  0  0
// 0  0  0  0   0  0  0  0   0  0  0  0   0  0  0  1   0  0  0  0   0  0  0  0   0  0  0  0  -1  0  0  0

// góc mịn
// 0  0 -1  1   0  0  0  0   0  0  0  0   0  0  0  0   1 -1  0  0   0  0  0  0   0  0  0  0   0  0  0  0
// 0  0  0  0   0  0 -1  1   0  0  0  0   0  0  0  0   0  0  0  0   1 -1  0  0   0  0  0  0   0  0  0  0
// 0  0  0  0   0  0  0  0   0  0 -1  1   0  0  0  0   0  0  0  0   0  0  0  0   1 -1  0  0   0  0  0  0
// 0  0  0  0   0  0  0  0   0  0  0  0   0  0 -1  1   0  0  0  0   0  0  0  0   0  0  0  0   1 -1  0  0

void ketNoiNgang_32x8( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau ) {
   
   // ---- cùng điểm
   unsigned int diaChi0 = soHangDau*beRongMaTranDich + soCotDau + 3;
   unsigned int diaChi1 = soHangDau*beRongMaTranDich + soCotDau + 16;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 4;
   diaChi1 += beRongMaTranDich + 4;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 4;
   diaChi1 += beRongMaTranDich + 4;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 4;
   diaChi1 += beRongMaTranDich + 4;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;

   // ---- góc mịn
   diaChi0 = (soHangDau + 4)*beRongMaTranDich + soCotDau + 2;
   diaChi1 = (soHangDau + 4)*beRongMaTranDich + soCotDau + 16;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi0+1] = 1.0f;
   maTranDich[diaChi1] = 1.0f;
   maTranDich[diaChi1+1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 4;
   diaChi1 += beRongMaTranDich + 4;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi0+1] = 1.0f;
   maTranDich[diaChi1] = 1.0f;
   maTranDich[diaChi1+1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 4;
   diaChi1 += beRongMaTranDich + 4;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi0+1] = 1.0f;
   maTranDich[diaChi1] = 1.0f;
   maTranDich[diaChi1+1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 4;
   diaChi1 += beRongMaTranDich + 4;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi0+1] = 1.0f;
   maTranDich[diaChi1] = 1.0f;
   maTranDich[diaChi1+1] = -1.0f;
}

//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0  -1/6  0   0   0
// 1/2  0   0   0   1/2  0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0 -1/18 0   0
//  0  1/6  0   0    0  1/6  0   0

//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0 -1/18 0
//  0   0  1/6  0    0   0  1/6  0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0 -1/6
//  0   0   0  1/2   0   0   0  1/2

//-1/2  0   0   0   1/2  0   0   0
//  0   0   0   0  -1/6  0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0 -1/6  0   0    0  1/6  0   0
//  0   0   0   0    0 -1/18 0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0

//  0   0 -1/6  0    0   0  1/6  0
//  0   0   0   0    0   0 -1/18 0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0  -1/2  0   0   0  1/2
//  0   0   0   0    0   0   0 -1/6
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0


void ketNoiNgang_8x32( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau ) {
   
   unsigned int diaChi = (soHangDau + 2)*beRongMaTranDich + soCotDau + 4;
   maTranDich[diaChi] = -1.0f/6.0f;

   diaChi = (soHangDau + 3)*beRongMaTranDich + soCotDau;
   maTranDich[diaChi] = 0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
   
   diaChi = (soHangDau + 6)*beRongMaTranDich + soCotDau + 5;
   maTranDich[diaChi] = -1.0f/18.0f;
   diaChi = (soHangDau + 7)*beRongMaTranDich + soCotDau + 1;
   maTranDich[diaChi] = 1.0f/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;

   diaChi = (soHangDau + 10)*beRongMaTranDich + soCotDau + 6;
   maTranDich[diaChi] = -1.0f/18.0f;
   
   diaChi = (soHangDau + 11)*beRongMaTranDich + soCotDau + 2;
   maTranDich[diaChi] = 1.0f/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 14)*beRongMaTranDich + soCotDau + 7;
   maTranDich[diaChi] = -1.0f/6.0f;
   
   diaChi = (soHangDau + 15)*beRongMaTranDich + soCotDau + 3;
   maTranDich[diaChi] = 0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
   
   // ----
   diaChi = (soHangDau + 16)*beRongMaTranDich + soCotDau;
   maTranDich[diaChi] = -0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
   
   diaChi = (soHangDau + 17)*beRongMaTranDich + soCotDau + 4;
   maTranDich[diaChi] = -1.0f/6.0f;
   
   diaChi = (soHangDau + 20)*beRongMaTranDich + soCotDau + 1;
   maTranDich[diaChi] = -1.0/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 21)*beRongMaTranDich + soCotDau + 5;
   maTranDich[diaChi] = -1.0f/18.0f;
   
   diaChi = (soHangDau + 24)*beRongMaTranDich + soCotDau + 2;
   maTranDich[diaChi] = -1.0/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 25)*beRongMaTranDich + soCotDau + 6;
   maTranDich[diaChi] = -1.0f/18.0f;

   diaChi = (soHangDau + 28)*beRongMaTranDich + soCotDau + 3;
   maTranDich[diaChi] = -0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
   
   diaChi = (soHangDau + 29)*beRongMaTranDich + soCotDau + 7;
   maTranDich[diaChi] = -1.0f/6.0f;
}


#pragma mark ---- Kết Nối Dộc
// 0  0  0  0    0  0  0  0   0  0  0  0   1  0  0  0
// 0  0  0  0    0  0  0  0   0  0  0  0   0  1  0  0
// 0  0  0  0    0  0  0  0   0  0  0  0   0  0  1  0
// 0  0  0  0    0  0  0  0   0  0  0  0   0  0  0  1

// 0  0  0  0    0  0  0  0  -1  0  0  0   1  0  0  0
// 0  0  0  0    0  0  0  0   0 -1  0  0   0  1  0  0
// 0  0  0  0    0  0  0  0   0  0 -1  0   0  0  1  0
// 0  0  0  0    0  0  0  0   0  0  0 -1   0  0  0  1

void ketNoiNgang_16x8_matDuoi( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau ) {
   
   // ---- cùng điểm
   unsigned int diaChi0 = soHangDau*beRongMaTranDich + soCotDau + 12;
   maTranDich[diaChi0] = 1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = 1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = 1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = 1.0f;
   
   // ---- góc mịn
   diaChi0 = (soHangDau + 4)*beRongMaTranDich + soCotDau + 8;
   unsigned int diaChi1 = diaChi0 + 4;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi1] = 1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   diaChi1 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi1] = 1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   diaChi1 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi1] = 1.0f;

   diaChi0 += beRongMaTranDich + 1;
   diaChi1 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = -1.0f;
   maTranDich[diaChi1] = 1.0f;
}

//  0   0   0   0     0   0   0   0
//  0   0   0   0     0   0   0   0
//  0   0   0   0     0   0   0   0
//  0   0   0   0     0   0   0   0

//  0   0   0   0     0   0   0   0
//  0   0   0   0     0   0   0   0
//  0   0   0   0     0   0   0   0
//  0   0   0   0     0   0   0   0

//  0   0   0   0   -1/6  0    0    0
//  0   0   0   0    0  -1/18  0    0
//  0   0   0   0    0    0  -1/18  0
//  0   0   0   0    0    0    0  -1/6

// 1/2  0   0   0    1/2  0   0   0
//  0  1/6  0   0     0  1/6  0   0
//  0   0  1/6  0     0   0  1/6  0
//  0   0   0  1/2    0   0   0  1/2

void ketNoiNgang_8x16_matDuoi( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau ) {
   
   unsigned int diaChi = (soHangDau + 8)*beRongMaTranDich + soCotDau + 4;
   maTranDich[diaChi] = -1.0f/6.0f;
   
   diaChi = (soHangDau + 9)*beRongMaTranDich + soCotDau + 5;
   maTranDich[diaChi] = -1.0f/18.0f;
   
   diaChi = (soHangDau + 10)*beRongMaTranDich + soCotDau + 6;
   maTranDich[diaChi] = -1.0f/18.0f;
   
   diaChi = (soHangDau + 11)*beRongMaTranDich + soCotDau + 7;
   maTranDich[diaChi] = -1.0f/6.0f;
   
   // ---- góc mịn
   diaChi = (soHangDau + 12)*beRongMaTranDich + soCotDau;
   maTranDich[diaChi] = 0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;

   diaChi = (soHangDau + 13)*beRongMaTranDich + soCotDau + 1;
   maTranDich[diaChi] = 1.0f/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 14)*beRongMaTranDich + soCotDau + 2;
   maTranDich[diaChi] = 1.0f/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 15)*beRongMaTranDich + soCotDau + 3;
   maTranDich[diaChi] = 0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
}

//-1  0  0  0    0  0  0  0   0  0  0  0   0  0  0  0
// 0 -1  0  0    0  0  0  0   0  0  0  0   0  0  0  0
// 0  0 -1  0    0  0  0  0   0  0  0  0   0  0  0  0
// 0  0  0 -1    0  0  0  0   0  0  0  0   0  0  0  0

// 1  0  0  0   -1  0  0  0   0  0  0  0   0  0  0  0
// 0  1  0  0    0 -1  0  0   0  0  0  0   0  0  0  0
// 0  0  1  0    0  0 -1  0   0  0  0  0   0  0  0  0
// 0  0  0  1    0  0  0 -1   0  0  0  0   0  0  0  0
void ketNoiNgang_16x8_matTren( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau ) {
   
   // ---- cùng điểm
   unsigned int diaChi0 = soHangDau*beRongMaTranDich + soCotDau;
   maTranDich[diaChi0] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = -1.0f;
   
   // ---- góc mịn
   diaChi0 = (soHangDau + 4)*beRongMaTranDich + soCotDau;
   unsigned int diaChi1 = diaChi0 + 4;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   diaChi1 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   diaChi1 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
   
   diaChi0 += beRongMaTranDich + 1;
   diaChi1 += beRongMaTranDich + 1;
   maTranDich[diaChi0] = 1.0f;
   maTranDich[diaChi1] = -1.0f;
}

//-1/2  0   0   0    1/2  0   0   0
//  0 -1/6  0   0     0  1/6  0   0
//  0   0 -1/6  0     0   0  1/6  0
//  0   0   0 -1/2    0   0   0  1/2

//  0   0   0   0  -1/6  0    0    0
//  0   0   0   0    0 -1/18  0    0
//  0   0   0   0    0   0 -1/18   0
//  0   0   0   0    0   0    0  -1/6

//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0

//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
//  0   0   0   0    0   0   0   0
void ketNoiNgang_8x16_matTren( float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich, unsigned short soCotDau, unsigned short soHangDau ) {
   
   unsigned int diaChi = soHangDau*beRongMaTranDich + soCotDau;
   maTranDich[diaChi] = -0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
   
   diaChi = (soHangDau + 1)*beRongMaTranDich + soCotDau + 1;
   maTranDich[diaChi] = -1.0f/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 2)*beRongMaTranDich + soCotDau + 2;
   maTranDich[diaChi] = -1.0f/6.0f;
   diaChi += 4;
   maTranDich[diaChi] = 1.0f/6.0f;
   
   diaChi = (soHangDau + 3)*beRongMaTranDich + soCotDau + 3;
   maTranDich[diaChi] = -0.5f;
   diaChi += 4;
   maTranDich[diaChi] = 0.5f;
   
   // ----
   diaChi = (soHangDau + 4)*beRongMaTranDich + soCotDau + 4;
   maTranDich[diaChi] = -1.0f/6.0f;

   diaChi = (soHangDau + 5)*beRongMaTranDich + soCotDau + 5;
   maTranDich[diaChi] = -1.0f/18.0f;
   
   diaChi = (soHangDau + 6)*beRongMaTranDich + soCotDau + 6;
   maTranDich[diaChi] = -1.0f/18.0f;
   
   diaChi = (soHangDau + 7)*beRongMaTranDich + soCotDau + 7;
   maTranDich[diaChi] = -1.0f/6.0f;
}


#pragma mark ---- CHÉP
void chepGiaTriVaoMaTran( float giaTri, unsigned short beRongMaTranChep, unsigned short beCaoMaTranChep,
                         float *maTranDich, unsigned short beRongMaTranDich, unsigned short beCaoMaTranDich,
                         unsigned short soCotDau, unsigned short soHangDau ) {
   
   // ---- nếu số cột và hàng bắt đầu hơn bề rộng và bề cao ma trận đích, không cần chép gì cả
   if( maTranDich == NULL ) {
      printf( "chepMaTranVaoMaTran: maTranDich == NULL\n" );
      return;
   }
   
   // ---- chỉ chép được nếu ma trận chép ở trong phạm vi ma trận đích
   if( (soCotDau < beRongMaTranDich) && (soHangDau < beCaoMaTranDich) ) {
      
      // ---- tính số cột và số hàng cuối
      unsigned short soCotCuoi = soCotDau + beRongMaTranChep;
      unsigned short soHangCuoi = soHangDau + beCaoMaTranChep;
      if( soCotCuoi > beRongMaTranDich )
         soCotCuoi = beRongMaTranDich;
      
      if( soHangCuoi > beCaoMaTranDich )
         soHangCuoi = beCaoMaTranDich;
      
      // ---- chép giá trị phân tử ma trận chép vào ma trận đích
      unsigned short soHang = soHangDau;
      unsigned short soHangMaTranChep = 0;
      
      while( soHang < soHangCuoi  ) {
         
         unsigned short soCot = soCotDau;
         unsigned int diaChiMaTranDich = soHang*beRongMaTranDich + soCot;
         unsigned int diaChiMaTranChep = soHangMaTranChep*beRongMaTranChep;
         while( soCot < soCotCuoi ) {
            maTranDich[diaChiMaTranDich] = giaTri;
            diaChiMaTranDich++;
            diaChiMaTranChep++;
            soCot++;
         }
         soHangMaTranChep++;
         soHang++;
      }
   }
}

#pragma mark ---- CHIẾU MA TRẬN
void chieu_maTran( float *maTran, unsigned short beRong, unsigned short beCao ) {
 
   unsigned int chiSoMaTran = 0;
   unsigned short soHang = 0;
   while( soHang < beCao ) {
      unsigned short soCot = 0;
      while( soCot < beRong ) {
         printf( " %7.4f", maTran[chiSoMaTran] );
         soCot++;
         chiSoMaTran++;
      }
      soHang++;
      printf( "\n" );
   }
   printf( "\n" );
}


void chieu_maTranKyTu( float *maTran, unsigned short beRong, unsigned short beCao ) {
   
   unsigned int chiSoMaTran = 0;
   unsigned short soHang = 0;
   while( soHang < beCao ) {
      unsigned short soCot = 0;
      while( soCot < beRong ) {
         unsigned kyTu = maTran[chiSoMaTran];
         if( kyTu == 0 )
            printf( " " );
         else
            printf( "%c", kyTu );
         soCot++;
         chiSoMaTran++;
      }
      soHang++;
      printf( "\n" );
   }
   printf( "\n" );
}

