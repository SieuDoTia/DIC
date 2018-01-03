//  suKhacBietXam.c   // sựKhácBiệtXám.c
//  Phiên bản 1.2
//  Phát hành 2560/12/12
//  Cho ảnh PNG
//  Khởi đầu 2560/03/13
//
//  Xuất sự khác biệt màu xám

// So sánh hai ảnh và tính sự khác biệt giữa chung. Giúp biết xài mẫu vật đủ cho Blender.

#import <stdio.h>
#import <stdlib.h>
#import <math.h>
#import <zlib.h>   // tải zlib


#define kSAI  0
#define kDUNG 1

/* Điểm */
typedef struct {
   int x;             // vị trí điểm
   int y;
   char doc;         // dốc hướng X mặt độ sáng tại điểm trong ảnh
   float goc;
   float huongX;       // hướng đến điểm này từ điểm trước
   float huongY;       // hướng đến điểm này từ điểm trước
   unsigned char xuLyRoi;   // xử lý rồi, cho biết đả gặp điểm này rồi và nên bỏ qua nó
   unsigned char doSang;
//   float doLonDoc;
//   float huongDoc;
} Diem;

/* Chữ Nhật */
typedef struct {
   short trai;
   short phai;
   short duoi;
   short tren;
} ChuNhat;

typedef struct {
   Diem mangDiem[128];
   unsigned char cap;
   ChuNhat chuNhat;
} Net;

#define kSO_LUONG__HE_SO_BO_LOC_BLACKMAN 21  // <--- QUAN TRỌNG: PHẢI DÙNG SỐ LẺ NHE!

/* đọc tệp PNG BGRO */
unsigned char *docPNG_BGRO( char *duongTapTin, unsigned int *beRong, unsigned int *beCao, unsigned char *canLatMau );

/* tính sự khác biệt giữa hai hình */
unsigned char *tinhSuKhacBietXam( unsigned char *duongTapTin0, unsigned char *duongTapTin1, unsigned int beRong, unsigned int beCao, unsigned char daoNghich );

/* bộ lọc Blackman */
unsigned char *boLocAnh_Blackman( unsigned char *anh, unsigned int beRong, unsigned int beCao );

/* Tính hệ số bộ lọc Blackman */
float *tinhCacHeSoBoLocBlackman( unsigned short beRong);

/* bộ lọc Sobel */
unsigned char *boLocAnh_Sobel_ngang( unsigned char *anh, unsigned int beRong, unsigned int beCao );
unsigned char *boLocAnh_Sobel_doc( unsigned char *anh, unsigned int beRong, unsigned int beCao );

/* Bộ Lộc Gauss */
float *tinhHeSoChoBoLocGauss( float beRongHam, float doChinhXac, unsigned short *beDaiMang );
unsigned char *boLocAnh_Gauss( unsigned char *anh, unsigned int beRong, unsigned int beCao, float chenhLech );

/* Trung Bình */
unsigned char *boLocTrungBinh( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned short beRongBoLoc );

/* Trung Bình Dốc */
unsigned char *boLocTrungBinhDoc( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned short beRongBoLoc );

/* Giới Hạn */
 void gioiHan( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned char gioiHan );

/* Lật Ngược */
void lậtNgược( unsigned char *anh, unsigned int beRong, unsigned int beCao );

/* Chỉnh Chênh Lệch */
void chinhChenhLech( unsigned char *anh, unsigned int beRong, unsigned int beCao, float triSoChenLech );

/* Chỉnh Độ Sáng */
void chinhDoSang( unsigned char *anh, unsigned int beRong, unsigned int beCao, char giaTriTang );

/* vuông hóa 8x8 */
void vuongHoa_giaTriTrungBinh( unsigned char *anh, unsigned int beRong, unsigned int beCao );

/* Tìm Mặt Nạ */
ChuNhat tìmMatNa( unsigned char *anhMatNa, unsigned int beRong, unsigned int beCao, unsigned char gioiHan, unsigned short beRongBoLoc );

unsigned char *catAnh( unsigned char *anh, unsigned int beRong, unsigned beCao, ChuNhat *matNa );



void veDuong( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem diem0, Diem diem1, unsigned int mau );

/* Lưu ảnh PNG */
void luuAnhPNG( char *tenTep, unsigned char *suKhacBiet, unsigned int beRong, unsigned int beCao );

/* Tô Màu */
unsigned char *toMauAnh( unsigned char *anh, unsigned int beRong, unsigned int beCao );

int main( int argc, char **argv ) {

   if( argc > 2 ) {
      unsigned int beRongTep0;
      unsigned int beCaoTep0;
      unsigned char canLatMauTep0;
      
      unsigned char *duLieuTep0 = docPNG_BGRO( argv[1], &beRongTep0, &beCaoTep0, &canLatMauTep0 );

      
      unsigned int beRongTep1;
      unsigned int beCaoTep1;
      unsigned char canLatMauTep1;
      
      unsigned char *duLieuTep1 = docPNG_BGRO( argv[2], &beRongTep1, &beCaoTep1, &canLatMauTep1 );


      if( (beRongTep0 == beRongTep1) && (beCaoTep0 == beCaoTep1) ) {
         // ---- tính sự khác giữa hai ảnh
         unsigned char *anhSuKhacBiet = tinhSuKhacBietXam( duLieuTep0, duLieuTep1, beRongTep0, beCaoTep0, kDUNG );
         printf( "Đang bộ Lọc ảnh sự khác biệt\n" );
         unsigned char *anhBoLocTrungBinh = boLocTrungBinh( anhSuKhacBiet, beRongTep0, beCaoTep0, 75 );
         unsigned char *anhBoLocTrungBinhDoc = boLocTrungBinhDoc( anhBoLocTrungBinh, beRongTep0, beCaoTep0, 70  );

         unsigned char *anhToMau = toMauAnh( anhBoLocTrungBinhDoc, beRongTep0, beCaoTep0 );
         // ---- tìm mặt nạ: bộ l ̣c trung bình trước, sau tìm chổ trong ảnh ≥ giới hạn
 /*        printf( "Đang tìm mặt nạ ảnh\n" );
         unsigned char *anhMatNa0 = boLocTrungBinh( duLieuTep0, beRongTep0, beCaoTep0, 20 );
         unsigned char *anhMatNa1 = boLocTrungBinh( duLieuTep1, beRongTep1, beCaoTep1, 20 );
         ChuNhat matNa0 = tìmMatNa( anhMatNa0, beRongTep0, beCaoTep0, 120, 75 >> 1 );  // <---- dùng nữa bộ lọc để mịn hoá ảng sự khạc biệt
         ChuNhat matNa1 = tìmMatNa( anhMatNa1, beRongTep1, beCaoTep1, 120, 75 >> 1 );
         printf( "Mặt nạ ảnh %s: %d %d %d %d\n", argv[1], matNa0.trai, matNa0.phai, matNa0.duoi, matNa0.tren );
         printf( "Mặt nạ ảnh %s: %d %d %d %d\n", argv[2], matNa1.trai, matNa1.phai, matNa1.duoi, matNa1.tren );

         // ---- giao hai mặt nạ
         ChuNhat matNaGiao;
         if( matNa0.trai < matNa1.trai )
            matNaGiao.trai = matNa1.trai;
         else
            matNaGiao.trai = matNa0.trai;
         // ----
         if( matNa0.phai < matNa1.phai )
            matNaGiao.phai = matNa0.phai;
         else
            matNaGiao.phai = matNa1.phai;
         // ----
         if( matNa0.duoi < matNa1.duoi )
            matNaGiao.duoi = matNa1.duoi;
         else
            matNaGiao.duoi = matNa0.phai;
         
         if( matNa0.tren < matNa1.tren )
            matNaGiao.tren = matNa0.tren;
         else
            matNaGiao.tren = matNa1.tren;
         printf( "Mặt nạ giao hai ảnh: %d %d %d %d\n", matNaGiao.trai, matNaGiao.phai, matNaGiao.duoi, matNaGiao.tren );


         // ---- cắt hình
         unsigned char *anhCat = catAnh( anhBoLocTrungBinh, beRongTep0, beCaoTep0, &matNaGiao );
         luuAnhPNG( "AnhCat.png", anhCat, matNaGiao.phai - matNaGiao.trai, matNaGiao.tren - matNaGiao.duoi );
         */
  //       unsigned char *anhBoLocGauss = boLocAnh_Gauss( anhSuKhacBiet, beRongTep0, beCaoTep0, 10.0f );

//         unsigned char *anhBoLocNgang = boLocAnh_Sobel_ngang( anhBoLocTrungBinh, beRongTep0, beCaoTep0 );
//         unsigned char *anhBoLocDoc = boLocAnh_Sobel_doc( anhBoLocNgang, beRongTep0, beCaoTep0 );
//         latNguoc( anhBoLocDoc, beRongTep0, beCaoTep0 ) ;
 //        unsigned char *anhBoLoc = boLocAnh_Blackman( anhSuKhacBiet, beRongTep0, beCaoTep0 );
//        vuongHoa_giaTriTrungBinh( anhSuKhacBiet, beRongTep0, beCaoTep0 );
 //        chinhDoSang( anhBoLocTrungBinh, beRongTep0, beCaoTep0, -30 );
 //        chinhChenhLech( anhBoLocTrungBinh, beRongTep0, beCaoTep0, 50.0f );
 //        gioiHan( anhBoLocTrungBinh, beRongTep0, beCaoTep0, 135 );
 //      unsigned char *anhBoLoc2 = boLocAnh_Blackman( anhBoLoc, beRongTep0, beCaoTep0 );

         // ---- lưu sự khác
         char tenAnhSuKhacBiet[255];
         char *dauTen = tenAnhSuKhacBiet;
         unsigned char chiSo = 0;
         while( argv[1][chiSo] != 0x00 ) {
            *dauTen = argv[1][chiSo];
            dauTen++;
            chiSo++;
         }
         // ---- bỏ đuôi
         dauTen -= 4;
         *dauTen = '_';
         dauTen++;
         *dauTen = '_';
         dauTen++;
         chiSo = 0;
         while( argv[2][chiSo] != 0x00 ) {
            *dauTen = argv[2][chiSo];
            dauTen++;
            chiSo++;
         }
         *dauTen = 0x00;

         printf( "Tên két qủa: %s\n", tenAnhSuKhacBiet );
         luuAnhPNG( tenAnhSuKhacBiet, anhBoLocTrungBinhDoc, beRongTep0, beCaoTep0 );

         free( anhSuKhacBiet );
         free( anhBoLocTrungBinh );
//         free( anhBoLocNgang );
//         free( anhBoLocDoc );
//         free( anhBoLoc2 );
      }
      
      free( duLieuTep0 );
      free( duLieuTep1 );
   }
   else {
      // ---- phàn nàn không có hình
      printf( "Làm ơn gõ tên hai tệp PNG BGRO khác nào, cỡ kích bằng nhau.\n" );
   }
   return 1;
}


// ---- nó đảo nghịch kết qủa cho dễ thấy hơn
unsigned char *tinhSuKhacBietXam( unsigned char *duongTapTin0, unsigned char *duongTapTin1, unsigned int beRong, unsigned int beCao, unsigned char daoNghich ) {
   
   // ---- đệm cho ảnh sự khác biệt
   unsigned char *anhSuKhacBiet = malloc( beRong*beCao << 2 );
   
   unsigned int chiSo = 0;
   unsigned int chiSoCuoi = beRong*beCao << 2;
   while( chiSo < chiSoCuoi ) {
      anhSuKhacBiet[chiSo] = 0xff;
      chiSo++;
   }
   
   // ---- cho tính sự khác biệt giữa hai ảnh
   unsigned int giaTriSuKhac = 0.0f;
   
   // ---- cho tính phân phối
   unsigned int phanPhoiSuKhacBiet[256] = {
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, //  32
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, //  64
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, //  96
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, // 128
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, // 160
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, // 192
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, // 224
      0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, // 256
   };

   if( anhSuKhacBiet != NULL ) {
      unsigned int diaChiCuoi = beRong*beCao << 2;
      unsigned int diaChiAnh = 0;
      while( diaChiAnh < diaChiCuoi ) {
         // ---- kênh xanh
         short suKhacDo = duongTapTin0[diaChiAnh] - duongTapTin1[diaChiAnh];
         if( suKhacDo < 0 )
            suKhacDo = -suKhacDo;
//         printf( "%d - %d = suKhac %d\n", suKhac, duongTapTin0[diaChiAnh], duongTapTin1[diaChiAnh] );

         giaTriSuKhac += suKhacDo;

         // ---- kênh lục
         short suKhacLuc = duongTapTin0[diaChiAnh+1] - duongTapTin1[diaChiAnh+1];
         if( suKhacLuc < 0 )
            suKhacLuc = -suKhacLuc;
//         printf( "%d - %d = suKhac %d\n", suKhac, duongTapTin0[diaChiAnh+1], duongTapTin1[diaChiAnh+1] );

         giaTriSuKhac += suKhacLuc;

         // ---- kênh đỏ
         short suKhacXanh = duongTapTin0[diaChiAnh+2] - duongTapTin1[diaChiAnh+2];
         if( suKhacXanh < 0 )
            suKhacXanh = -suKhacXanh;
//         printf( "%d - %d = suKhac %d\n", suKhac, duongTapTin0[diaChiAnh+2], duongTapTin1[diaChiAnh+2] );

         giaTriSuKhac += suKhacXanh;
   
         // ---- tính trung bình sự khác biệt
 
         short suKhac = (suKhacDo + suKhacLuc + suKhacXanh)/3;
         phanPhoiSuKhacBiet[suKhac]++;

         if( daoNghich ) {
            anhSuKhacBiet[diaChiAnh] = 255 - suKhac;
            anhSuKhacBiet[diaChiAnh+1] = 255 - suKhac;
            anhSuKhacBiet[diaChiAnh+2] = 255 - suKhac;
         }
         else {
            anhSuKhacBiet[diaChiAnh] = suKhac;
            anhSuKhacBiet[diaChiAnh+1] = suKhac;
            anhSuKhacBiet[diaChiAnh+2] = suKhac;
         }
         
         // ---- kênh đục
         anhSuKhacBiet[diaChiAnh+3] = 0xff;
         diaChiAnh += 4;
      }
      
      unsigned soLuongDuLieu = beRong*beCao*3;
      printf( "Phân Phối:\n" );

      unsigned char chiSo = 0;
      while( chiSo < 20 ) {
         printf( "%3d: %7.5f\n", chiSo, (float)phanPhoiSuKhacBiet[chiSo]/(float)soLuongDuLieu );
         chiSo++;
      }
      printf( "Sự khác: %5.3f\n", ((float)giaTriSuKhac/(float)(soLuongDuLieu)) );

   }
   else
      printf( "Sự khác: tinhSuKhacBiet: Vấn đệ tạo đệm cho ảnh sự khác biệt\n" );
   
   return anhSuKhacBiet;
}

#pragma mark ---- Bộ Lọc Blackman
#define kA_0 0.42f
#define kA_1 0.50f
#define kA_2 0.08f

#define kTI_SO_VONG_TRON 3.1415926f

// ---- LƯU Ý: Hai hệ số đầu và cuối = 0 và không lơu hay tính nó
float *tinhCacHeSoBoLocBlackman( unsigned short beRongBoLoc ) {

   // ---- mẫu số của phân số trong cộng thức, chỉ cần tính một lần
   float mauSo = (float)(beRongBoLoc - 1);
   
   // ---- tạo mảng hệ số Blackman
   unsigned short beRongBoLoc_khongBangKhong = beRongBoLoc - 2;
   float *mangHeSoBlackman = malloc( sizeof(float)*(beRongBoLoc_khongBangKhong) );
   
   // ---- tính các hệ số KHÔNG = 0
   float doLon = 0.0f;
   if( mangHeSoBlackman != NULL ) {
      unsigned short chiSo = 0;
      while( chiSo < beRongBoLoc_khongBangKhong ) {
         mangHeSoBlackman[chiSo] = kA_0 - kA_1*cosf(2.0f*kTI_SO_VONG_TRON*(chiSo+1)/mauSo) + kA_2*cosf(4.0f*kTI_SO_VONG_TRON*(chiSo+1)/mauSo);
         doLon += mangHeSoBlackman[chiSo];
         chiSo++;
      }
      
      // ---- đơn vị hóa
      chiSo = 0;

      while( chiSo < beRongBoLoc_khongBangKhong ) {
         mangHeSoBlackman[chiSo] /= doLon;
//         printf( "%d %5.3f\n", chiSo, mangHeSoBlackman[chiSo] );
         chiSo++;
      }
   }
   else {
      printf( "tinhCacTroSoBoLocBlackman: vấn đề tạo mảng hệ số Blackman\n" );
      exit(0);
   }

   return mangHeSoBlackman;
}

unsigned char *boLocAnh_Blackman( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {
   
   // ---- tạo bộ lọc
   float *boLocAnh_Blackman = tinhCacHeSoBoLocBlackman( kSO_LUONG__HE_SO_BO_LOC_BLACKMAN );
   unsigned short beRongBoLoc_khongBangKhong = kSO_LUONG__HE_SO_BO_LOC_BLACKMAN - 2;
   
   unsigned char *anhBoLoc = malloc( beRong*beCao << 2 );

   if( anhBoLoc ) {
      unsigned int soHang = beRongBoLoc_khongBangKhong >> 1;
      unsigned int soHangCuoi = beCao - (beRongBoLoc_khongBangKhong >> 1);
      int cachMotHang = beRong << 2;   // số lượng byte giữa một đến hàng tiếp, cùng cột
      
      while( soHang < soHangCuoi ) {
         
         unsigned short soCot = 0;
         unsigned short soCotCuoi = beRong;

         while( soCot < soCotCuoi ) {
   
            // ---- áp dụng bộ lọc
            int diaChiAnhBoLoc = (beRong*soHang + soCot) << 2;
            int diaChiAnh = diaChiAnhBoLoc - cachMotHang*(beRongBoLoc_khongBangKhong >> 1);

            // ---- xóa điểm ảnh
            anhBoLoc[diaChiAnhBoLoc] = 0;
            anhBoLoc[diaChiAnhBoLoc+1] = 0;
            anhBoLoc[diaChiAnhBoLoc+2] = 0;
            anhBoLoc[diaChiAnhBoLoc+3] = 0xff;   // độ đục

            unsigned short giaTriLocXanh = 0;
            unsigned short giaTriLocLuc = 0;
            unsigned short giaTriLocDo = 0;
            
            unsigned short chiSoHeSo = 0;
            
            while( chiSoHeSo < beRongBoLoc_khongBangKhong ) {
//               printf( "%d/%d, %d/%d %d: diaChiAnh %d - diaChiAnhBoLoc %d = %d  %5.3f\n", soHang, beCao, soCot, beRong, chiSoHeSo, diaChiAnh, diaChiAnhBoLoc, diaChiAnh - diaChiAnhBoLoc, boLocAnh_Blackman[chiSoHeSo] );
               giaTriLocXanh += anh[diaChiAnh]*boLocAnh_Blackman[chiSoHeSo];
               giaTriLocLuc += anh[diaChiAnh+1]*boLocAnh_Blackman[chiSoHeSo];
               giaTriLocDo += anh[diaChiAnh+2]*boLocAnh_Blackman[chiSoHeSo];
               diaChiAnh += cachMotHang;
  
               chiSoHeSo++;
            }
            anhBoLoc[diaChiAnhBoLoc] = giaTriLocXanh;
            anhBoLoc[diaChiAnhBoLoc+1] = giaTriLocLuc;
            anhBoLoc[diaChiAnhBoLoc+2] = giaTriLocDo;
            anhBoLoc[diaChiAnhBoLoc+3] = 0xff;

 //           printf( "%d %d %d\n", soCot, soHang, anhBoLoc[diaChiAnhBoLoc] );
            // ---- điểm ảnh tiếp
            soCot++;
         };
         soHang++;
      }
   }
   else {
      printf( "Sự khác: boLocAnh_Blackman: Vấn đệ tạo đệm cho sự khác biệt\n" );
   }
   return anhBoLoc;
}

#pragma mark ---- Chỉnh Chênh Lệch
void chinhChenhLech( unsigned char *anh, unsigned int beRong, unsigned int beCao, float triSoChenLech ) {
   
   // ---- tính giá trị chỉnh
   float giaTriChinh = 259.0f*(triSoChenLech + 255.0f)/(255.0f*(259.0f - triSoChenLech));
   
   unsigned int diaChiAnh = 0;
   unsigned int soHang = 0;
   while( soHang < beCao ) {
      unsigned short soCot = 0;
      while( soCot < beRong ) {
         // ----
         signed short ketQua = (anh[diaChiAnh] - 128)*giaTriChinh + 128;
         if( ketQua < 0 )
            ketQua = 0;
         else if( ketQua > 255 )
            ketQua = 255;
         anh[diaChiAnh] = ketQua;
         diaChiAnh++;
         // ----
         ketQua = (anh[diaChiAnh] - 128)*giaTriChinh + 128;
         if( ketQua < 0 )
            ketQua = 0;
         else if( ketQua > 255 )
            ketQua = 255;
         anh[diaChiAnh] = ketQua;
         diaChiAnh++;
         // ----
         ketQua =  (anh[diaChiAnh] - 128)*giaTriChinh + 128;
         if( ketQua < 0 )
            ketQua = 0;
         else if( ketQua > 255 )
            ketQua = 255;
         anh[diaChiAnh] = ketQua;
         diaChiAnh+= 2;    // bỏ kênh độ đục

         soCot++;
      }
      soHang++;
   }
}

#pragma mark ----
void chinhDoSang( unsigned char *anh, unsigned int beRong, unsigned int beCao, char triSoTang ) {
   
   unsigned int diaChiAnh = 0;
   unsigned int diaChiCuoi = beRong * beCao << 2;
   while( diaChiAnh < diaChiCuoi ) {
      // ---- ính giá trị chỉnh
      short giaTriChinh = anh[diaChiAnh] + triSoTang;
      if( giaTriChinh < 0 )
         giaTriChinh = 0;
      else if( giaTriChinh > 255 )
         giaTriChinh = 255;
      anh[diaChiAnh] += giaTriChinh;
      
      // ----
      giaTriChinh = anh[diaChiAnh+1] + triSoTang;
      if( giaTriChinh < 0 )
         giaTriChinh = 0;
      else if( giaTriChinh > 255 )
         giaTriChinh = 255;
      anh[diaChiAnh+1] += giaTriChinh;
      
      // ----
      giaTriChinh = anh[diaChiAnh+2] + triSoTang;
      if( giaTriChinh < 0 )
         giaTriChinh = 0;
      else if( giaTriChinh > 255 )
         giaTriChinh = 255;
      anh[diaChiAnh+2] += giaTriChinh;
      
      diaChiAnh += 4;
   }
}


#pragma mark ---- Vuông Hóa
/*   +------+
     |      |
     |      |
     +------+   */
unsigned int tìmGiaTriTrungBinhTrongO( unsigned char *anh, unsigned int beRong, unsigned int soCotDau,
                           unsigned int soHangDau, unsigned int soCotCuoi, unsigned int soHangCuoi );
void toDiemAnhThapTrongO( unsigned int diemAnhThap, unsigned char *anh, unsigned int beRong, unsigned int soCotDau,
                         unsigned int soHangDau, unsigned int soCotCuoi, unsigned int soHangCuoi );

void vuongHoa_giaTriTrungBinh( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {

   
   unsigned int soHangDau = 0;
   while( soHangDau < beCao ) {
      // ----
      unsigned int soHangCuoi = soHangDau + 32;
      if( soHangCuoi > beCao )
         soHangCuoi = beCao;

      unsigned int soCotDau = 0;
      while( soCotDau < beRong ) {
          unsigned int soCotCuoi = soCotDau + 32;
          if( soCotCuoi > beRong )
            soCotCuoi = beRong;

         // ---- kiếm điểm ảnh có giá trị thấp nhất
         unsigned int diemAnhThap = tìmGiaTriTrungBinhTrongO( anh, beRong, soCotDau, soHangDau, soCotCuoi, soHangCuoi );

         if( diemAnhThap == 0xfc )
            exit(0);

         toDiemAnhThapTrongO( diemAnhThap, anh, beRong, soCotDau, soHangDau, soCotCuoi, soHangCuoi );
         // ---- tô màu
         soCotDau += 32;
      }
      soHangDau += 32;
   }
}

unsigned int tìmGiaTriTrungBinhTrongO( unsigned char *anh, unsigned int beRong, unsigned int soCotDau,
                                        unsigned int soHangDau, unsigned int soCotCuoi, unsigned int soHangCuoi ) {
   
   unsigned int soHang = soHangDau;
   unsigned int giaTriXanh = 0;
   unsigned int giaTriLuc = 0;
   unsigned int giaTriDo = 0;
   
   unsigned int soLuongDiemAnh = 0;

   while( soHang < soHangCuoi ) {
      unsigned int soCot = soCotDau;
      unsigned int diaChiAnh = (beRong*soHang + soCot) << 2;
      while( soCot < soCotCuoi ) {

         giaTriXanh += anh[diaChiAnh];
         giaTriLuc += anh[diaChiAnh+1];
         giaTriDo += anh[diaChiAnh+2];

         soLuongDiemAnh++;
         diaChiAnh += 4;
         soCot++;
      }
      soHang++;
   }
   giaTriXanh /= soLuongDiemAnh;
   giaTriLuc /= soLuongDiemAnh;
   giaTriDo /= soLuongDiemAnh;

   return giaTriXanh | (giaTriLuc & 0xff) << 8 | (giaTriDo & 0xff)  << 16;
}

void toDiemAnhThapTrongO( unsigned int diemAnhThap, unsigned char *anh, unsigned int beRong, unsigned int soCotDau,
                                unsigned int soHangDau, unsigned int soCotCuoi, unsigned int soHangCuoi ) {
   
   // ---- các giá trị của điểm ảnh thấp
   unsigned char mauXanh = diemAnhThap & 0xff;
   unsigned char mauLuc = (diemAnhThap >> 8) & 0xff;
   unsigned char mauDo = (diemAnhThap >> 16) & 0xff;

   unsigned int soHang = soHangDau;
   while( soHang < soHangCuoi ) {

      unsigned int soCot = soCotDau;
      unsigned int diaChiAnh = (beRong*soHang + soCot) << 2;
      
      while( soCot < soCotCuoi ) {

         anh[diaChiAnh] = mauXanh;
         anh[diaChiAnh+1] = mauLuc;
         anh[diaChiAnh+2] = mauDo;

         diaChiAnh += 4;
         soCot++;
      }
      soHang++;
   }
}

#pragma mark ---- BỘ LỘC SOBEL
/*
    -1 0 1
    -2 0 2
    -1 0 1
 */
unsigned char *boLocAnh_Sobel_ngang( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {
   
   unsigned char *anhBoLoc = calloc( beRong*beCao, 4 );
   
   if( anhBoLoc ) {
      
      unsigned int soHang = 1;
      unsigned int soHangCuoi = beCao - 1;
      
      while( soHang < soHangCuoi ) {
         unsigned int diaChiAnhHangDuoi = ((soHang-1)*beRong + 1) << 2;
         unsigned int diaChiAnhHangGiua = diaChiAnhHangDuoi + (beRong << 2);
         unsigned int diaChiAnhHangTren = diaChiAnhHangGiua + (beRong << 2);
         
         unsigned short soCot = 1;
         unsigned short soCotCuoi = beRong - 1;

         while( soCot < soCotCuoi ) {
            
            // ---- áp dụng bộ lọc
            short giaTri = anh[diaChiAnhHangDuoi + 4] - anh[diaChiAnhHangDuoi - 4];
            giaTri += (anh[diaChiAnhHangGiua + 4] - anh[diaChiAnhHangDuoi - 4]) << 1;
            giaTri += anh[diaChiAnhHangTren + 4] - anh[diaChiAnhHangTren - 4];
            giaTri >>= 3;
            anhBoLoc[diaChiAnhHangGiua] = giaTri;
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;

            // ----
            giaTri = anh[diaChiAnhHangDuoi + 4] - anh[diaChiAnhHangDuoi - 4];
            giaTri += (anh[diaChiAnhHangGiua + 4] - anh[diaChiAnhHangDuoi - 4]) << 1;
            giaTri += anh[diaChiAnhHangTren + 4] - anh[diaChiAnhHangTren - 4];
            giaTri >>= 3;
            anhBoLoc[diaChiAnhHangGiua] = giaTri;
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;
            
            // ----
            giaTri = anh[diaChiAnhHangDuoi + 4] - anh[diaChiAnhHangDuoi - 4];
            giaTri += (anh[diaChiAnhHangGiua + 4] - anh[diaChiAnhHangDuoi - 4]) << 1;
            giaTri += anh[diaChiAnhHangTren + 4] - anh[diaChiAnhHangTren - 4];
            giaTri >>= 3;
            anhBoLoc[diaChiAnhHangGiua] = giaTri;
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;

            // ----
            anhBoLoc[diaChiAnhHangGiua] = 0xff;
            anhBoLoc[diaChiAnhHangGiua] = 0xff;
            anhBoLoc[diaChiAnhHangGiua] = 0xff;

            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;

            soCot++;
         };
         soHang++;

      }
               
   }
   else {
      printf( "Sự khác: boLocAnh_Blackman: Vấn đệ tạo đệm cho sự khác biệt\n" );
   }
   return anhBoLoc;
}

/*
  1  2  1
  0  0  0
 -1 -2 -1
 */
unsigned char *boLocAnh_Sobel_doc( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {
   
   unsigned char *anhBoLoc = calloc( beRong*beCao, 4 );
   
   if( anhBoLoc ) {
      
      unsigned int soHang = 1;
      unsigned int soHangCuoi = beCao - 1;
      
      while( soHang < soHangCuoi ) {
         unsigned int diaChiAnhHangDuoi = ((soHang-1)*beRong + 1) << 2;
         unsigned int diaChiAnhHangGiua = diaChiAnhHangDuoi + (beRong << 2);
         unsigned int diaChiAnhHangTren = diaChiAnhHangGiua + (beRong << 2);
         
         unsigned short soCot = 1;
         unsigned short soCotCuoi = beRong - 1;
         
         while( soCot < soCotCuoi ) {
            
            // ---- áp dụng bộ lọc
            short giaTri = anh[diaChiAnhHangDuoi - 4]  - (anh[diaChiAnhHangDuoi + 4] << 1) - anh[diaChiAnhHangDuoi - 4];
            giaTri += anh[diaChiAnhHangTren - 4] + (anh[diaChiAnhHangTren] << 1) - anh[diaChiAnhHangTren - 4];
            giaTri >>= 3;
            anhBoLoc[diaChiAnhHangGiua] = giaTri;
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;
            
            // ----
            giaTri = anh[diaChiAnhHangDuoi - 4] - (anh[diaChiAnhHangDuoi + 4] << 1) - anh[diaChiAnhHangDuoi - 4];
            giaTri += anh[diaChiAnhHangTren - 4] + (anh[diaChiAnhHangTren] << 1) - anh[diaChiAnhHangTren - 4];
            giaTri >>= 3;
            anhBoLoc[diaChiAnhHangGiua] = giaTri;
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;
            
            // ----
            giaTri = anh[diaChiAnhHangDuoi - 4] - (anh[diaChiAnhHangDuoi + 4] << 1) - anh[diaChiAnhHangDuoi - 4];
            giaTri += anh[diaChiAnhHangTren - 4] + (anh[diaChiAnhHangTren] << 1) - anh[diaChiAnhHangTren - 4];
            giaTri >>= 3;
            anhBoLoc[diaChiAnhHangGiua] = giaTri;
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;
            
            // ----
            anhBoLoc[diaChiAnhHangGiua] = 0xff;
            anhBoLoc[diaChiAnhHangGiua] = 0xff;
            anhBoLoc[diaChiAnhHangGiua] = 0xff;
            
            diaChiAnhHangDuoi++;
            diaChiAnhHangGiua++;
            diaChiAnhHangTren++;
            
            soCot++;
         };
         soHang++;
         
      }
   }
   else {
      printf( "Sự khác: boLocAnh_Blackman: Vấn đệ tạo đệm cho sự khác biệt\n" );
   }
   return anhBoLoc;
}

#pragma mark ---- Bộ Lọc Gauss
//#define kHAI_PI 6.28f
#define kDO_CHINH_XAC 0.01f

// f(x) = (1/d) * expf( -c^2/b^2 )
// đcx = (1/d) * expf( -c^2/b^2 )
//   đcx*d = expf( -c^2/b^2 )
//   ln|đcx*d| = -c^2/b^2
//  sqrt( -ln|đcx*d| ) = c/b
//   c = sqrt(ln|đcx*d|)*b
// d = (2πb^2)


#include <math.h>
float *tinhHeSoChoBoLocGauss( float beRongHam, float doChinhXac, unsigned short *beDaiMang ) {
   
   // ---- tính bề rộng bên
   float beRongHamBinh = beRongHam * beRongHam;
   
   // ---- tính số lượng hệ số
   short soLuongHeSo = ceilf( sqrt( -log( kDO_CHINH_XAC ) )*beRongHam );//doChinhXac/
   float canSoBeRongHam = sqrtf( beRongHam );
   // nhân 2 vì có hai bền
   float toaDo = -(soLuongHeSo);
   
   soLuongHeSo <<= 1; // nhân 2 (hai bên)
   soLuongHeSo++;     // thêm một cái cho số không
   
   // ---- tạo mảng hệ số
   float *mangHeSo = malloc( soLuongHeSo );
   if( mangHeSo ) {
      
      // ---- tính hệ số
      //          -
      //        /   \
      //    --/       \--
      // +--------+--------+
      float tongCong = 0.0f;
      unsigned short chiSoHeSo = 0;
      while( chiSoHeSo < soLuongHeSo ) {
         mangHeSo[chiSoHeSo] = expf( -0.5f*toaDo*toaDo/beRongHamBinh );
         printf( "%d  %5.3f\n", chiSoHeSo, mangHeSo[chiSoHeSo] );
         toaDo += 1.0f;
         tongCong += mangHeSo[chiSoHeSo];
         chiSoHeSo++;
      }
   
     // ---- đơn vị hóa
      chiSoHeSo = 0;
      while( chiSoHeSo < soLuongHeSo ) {
         mangHeSo[chiSoHeSo] /= tongCong;
         printf( "%d  %5.3f\n", chiSoHeSo, mangHeSo[chiSoHeSo] );
         chiSoHeSo++;
      }
      *beDaiMang = soLuongHeSo;
      
      return mangHeSo;
   }
   else {
      printf( "tinhHeSoChoBoLocGauss: có vấn đề dành trí nhớ cho hệ số bộ lọc Gauss\n" );
      return NULL;
   }
}

unsigned char *boLocAnh_Gauss( unsigned char *anh, unsigned int beRong, unsigned int beCao, float chenhLech ) {
   
   // ---- tạo bộ lọc
   unsigned short soLuongHeSo = 0;
   float *boLocGauss = tinhHeSoChoBoLocGauss( chenhLech, kDO_CHINH_XAC, &soLuongHeSo );
   unsigned short nuaBeRongBoLoc = soLuongHeSo >> 1;

   unsigned char *anhBoLoc0 = malloc( beRong*beCao << 2 );
   unsigned char *anhBoLoc1 = malloc( beRong*beCao << 2 );
   
   unsigned int chiSo = 0;
   unsigned int chiSoCuoi = beRong*beCao << 2;
   while( chiSo < chiSoCuoi ) {
      anhBoLoc0[chiSo] = 0xff;
      anhBoLoc1[chiSo] = 0xff;
      chiSo++;
   }
   
   if( anhBoLoc0 && anhBoLoc1 ) {
      unsigned int soHang = 0;
      unsigned int soHangCuoi = beCao;
      unsigned short soCotCuoi = beRong - nuaBeRongBoLoc;
      int cachMotHang = beRong << 2;   // số lượng byte giữa một đến hàng tiếp, cùng cột

      // ==== hượng ngang
      while( soHang < soHangCuoi ) {
         
         unsigned short soCot = nuaBeRongBoLoc;

         
         while( soCot < soCotCuoi ) {

            // ---- áp dụng bộ lọc
            int diaChiAnh = (beRong*soHang + soCot) << 2;

            unsigned short giaTriLocXanh = 0;
            unsigned short giaTriLocLuc = 0;
            unsigned short giaTriLocDo = 0;
            
            int viTriTuongDoi = -(nuaBeRongBoLoc << 2);
            unsigned short chiSoHeSo = 0;
            
            while( chiSoHeSo < soLuongHeSo ) {
//               printf( " viTri %d  %d\n", viTriTuongDoi, diaChiAnh );
               giaTriLocXanh += anh[diaChiAnh - viTriTuongDoi]*boLocGauss[chiSoHeSo];
               giaTriLocLuc += anh[diaChiAnh+1 - viTriTuongDoi]*boLocGauss[chiSoHeSo];
               giaTriLocDo += anh[diaChiAnh+2 - viTriTuongDoi]*boLocGauss[chiSoHeSo];
               viTriTuongDoi += 4;
               chiSoHeSo++;
            }

            anhBoLoc0[diaChiAnh] = giaTriLocXanh;
            anhBoLoc0[diaChiAnh+1] = giaTriLocLuc;
            anhBoLoc0[diaChiAnh+2] = giaTriLocDo;
            anhBoLoc0[diaChiAnh+3] = 0xff;

            // ---- điểm ảnh tiếp
            soCot++;
         };
         soHang++;
      }
      
      // ==== hượng dọc
      soHang = nuaBeRongBoLoc;
      soHangCuoi = beCao - nuaBeRongBoLoc;
      soCotCuoi = beRong;
      
      while( soHang < soHangCuoi ) {
         
         unsigned short soCot = 0;

         while( soCot < soCotCuoi ) {
//            printf( "d %d %d\n", soCot, soHang );
            // ---- áp dụng bộ lọc
            unsigned int diaChiAnh = (beRong*soHang + soCot) << 2;
            
            unsigned short giaTriLocXanh = 0;
            unsigned short giaTriLocLuc = 0;
            unsigned short giaTriLocDo = 0;
            
            short viTriTuongDoi = -(nuaBeRongBoLoc*beRong << 2);
            unsigned short chiSoHeSo = 0;
            
            while( chiSoHeSo < soLuongHeSo ) {
               //               printf( "%d/%d, %d/%d %d: diaChiAnh %d - diaChiAnhBoLoc %d = %d  %5.3f\n", soHang, beCao, soCot, beRong, chiSoHeSo, diaChiAnh, diaChiAnhBoLoc, diaChiAnh - diaChiAnhBoLoc, boLocAnh_Blackman[chiSoHeSo] );
               giaTriLocXanh += anhBoLoc0[diaChiAnh - viTriTuongDoi]*boLocGauss[chiSoHeSo];
               giaTriLocLuc += anhBoLoc0[diaChiAnh+1 - viTriTuongDoi]*boLocGauss[chiSoHeSo];
               giaTriLocDo += anhBoLoc0[diaChiAnh+2 - viTriTuongDoi]*boLocGauss[chiSoHeSo];
               viTriTuongDoi += beRong << 2;
               chiSoHeSo++;
            }
            
            anhBoLoc1[diaChiAnh] = giaTriLocXanh;
            anhBoLoc1[diaChiAnh+1] = giaTriLocLuc;
            anhBoLoc1[diaChiAnh+2] = giaTriLocDo;
            anhBoLoc1[diaChiAnh+3] = 0xff;

            // ---- điểm ảnh tiếp
            soCot++;
         };
         soHang++;
      }

   }
   else {
      printf( "Sự khác: boLocAnh_Blackman: Vấn đệ tạo đệm cho sự khác biệt\n" );
   }
   return anhBoLoc1;
}

#pragma mark ---- Bộ Lọc Trung Bình
unsigned char *boLocTrungBinh( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned short beRongBoLoc ) {
   
   // ---- mẫu số để "đơn vị hóa" giá trị từ bộ lọc (0 đến 255)
   unsigned char *anhBoLoc = malloc( beRong*beCao << 2 );
   unsigned char *anhBoLoc1 = malloc( beRong*beCao << 2 );
   unsigned short phanNuaBoLoc = beRongBoLoc >> 1;
   
   //
   unsigned int chiSo = 0;
   unsigned int chiSoCuoi = beRong*beCao << 2;
   while( chiSo < chiSoCuoi ) {
      anhBoLoc[chiSo] = 0xff;
      anhBoLoc1[chiSo] = 0xff;
      chiSo++;
   }
   
   if( anhBoLoc && anhBoLoc1 ) {
      // ================ bộ lọc hượng ngang
      unsigned int soHang = 0;
      unsigned int soHangCuoi = beCao;
      
      // ---- địa chỉ đầu hảng trong ảnh
      int diaChiAnh = beRong*soHang << 2;
      
      while( soHang < soHangCuoi ) {
         
         // ==== dùng điểm ảnh đầu cho phần bộ lọc ở ngoài ảnh
         //         +---------------------  ảnh
         //           ^
         //           |
         //      +----+----+  bộ lọc
         unsigned char giaTriAnhCanhTrai = anh[diaChiAnh];
         unsigned int soCot = 0;
         while( soCot < phanNuaBoLoc ) {
            unsigned short soLuongDiemAnhNgoai = (phanNuaBoLoc - soCot);
            unsigned int giaTriLocXanh = soLuongDiemAnhNgoai*giaTriAnhCanhTrai;
            unsigned int giaTriLocLuc = soLuongDiemAnhNgoai*giaTriAnhCanhTrai;
            unsigned int giaTriLocDo = soLuongDiemAnhNgoai*giaTriAnhCanhTrai;
            
            // ---- chỉ số trong bộ lọc (phạm vi trong 0 tới beRongBoLoc)
            short chiSoCotBoLoc = phanNuaBoLoc - soCot;
            
            // ---- địa chỉ trong ảnh để bắt đầu bộ lọc
            int diaChiDeBoLoc = diaChiAnh - (soCot << 2);
            
            //            printf( "%d giaTriAnhCanhTrai %d  soLuongDiemAnhNgoai %d  diaChiAnh %d\n",  soCot, giaTriAnhCanhTrai, soLuongDiemAnhNgoai, diaChiAnh  );
            while( chiSoCotBoLoc < beRongBoLoc ) {
               
               giaTriLocDo += anh[diaChiDeBoLoc];
               giaTriLocLuc += anh[diaChiDeBoLoc+1];
               giaTriLocXanh += anh[diaChiDeBoLoc+2];
               chiSoCotBoLoc++;
               diaChiDeBoLoc += 4;
            }
            
            anhBoLoc[diaChiAnh] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc[diaChiAnh+2] = giaTriLocDo/beRongBoLoc;
            anhBoLoc[diaChiAnh+3] = 0xff;
            
            diaChiAnh += 4;
            soCot++;
         }
         
         // ==== trong ảnh có đủ điểm ảnh
         unsigned short soCotCuoi = beRong - phanNuaBoLoc;
         soCot = phanNuaBoLoc;
         // ---- áp dụng bộ lọc
         diaChiAnh = (beRong*soHang + soCot) << 2;
         
         while( soCot < soCotCuoi ) {
            
            unsigned int giaTriLocXanh = 0;
            unsigned int giaTriLocLuc = 0;
            unsigned int giaTriLocDo = 0;
            
            // ---- chỉ số trong bộ lọc
            short chiSoCotBoLoc = 0;
            // ---- cột tương đối với điểm ảnh
            short cotTuongDoi = -phanNuaBoLoc;
            // ---- tính địa chỉ đổ bộ lọc trong ảnh
            int diaChiDeBoLoc = diaChiAnh + (cotTuongDoi << 2);
            
            while( chiSoCotBoLoc < beRongBoLoc ) {
               
               giaTriLocDo += anh[diaChiDeBoLoc];
               giaTriLocLuc += anh[diaChiDeBoLoc+1];
               giaTriLocXanh += anh[diaChiDeBoLoc+2];
               chiSoCotBoLoc++;
               diaChiDeBoLoc += 4;
            }
            //           printf( "%d  diaChiAnh %d\n",  soCot, diaChiAnh  );
            anhBoLoc[diaChiAnh] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc[diaChiAnh+2] = giaTriLocDo/beRongBoLoc;
            anhBoLoc[diaChiAnh+3] = 0xff;
            
            // ---- điểm ảnh tiếp
            soCot++;
            diaChiAnh += 4;
         };
         
         // ==== dùng điểm ảnh đầu cho phân` bộ lọc ở ngoài ảnh
         //  ------------+  ảnh
         //           ^
         //           |
         //      +----+----+  bộ lọc
         unsigned char giaTriAnhCanhPhai = anh[diaChiAnh + ((beRong - 1) << 2)];
         while( soCot < beRong ) {
            unsigned short soLuongDiemAnhNgoai = phanNuaBoLoc - (beRong - soCot - 1);
            unsigned int giaTriLocXanh = soLuongDiemAnhNgoai*giaTriAnhCanhPhai;
            unsigned int giaTriLocLuc = soLuongDiemAnhNgoai*giaTriAnhCanhPhai;
            unsigned int giaTriLocDo = soLuongDiemAnhNgoai*giaTriAnhCanhPhai;
            
            // ---- chỉ số trong bộ lọc (phạm vi trong 0 tới beRongBoLoc)
            short chiSoCotBoLoc = 0;
            
            // ---- cột tương đối với điểm ảnh
            short cotTuongDoi = -phanNuaBoLoc;
            int diaChiDeBoLoc = diaChiAnh + (cotTuongDoi << 2);
            
            while( chiSoCotBoLoc < beRongBoLoc - soLuongDiemAnhNgoai ) {
               
               giaTriLocDo += anh[diaChiDeBoLoc];
               giaTriLocLuc += anh[diaChiDeBoLoc+1];
               giaTriLocXanh += anh[diaChiDeBoLoc+2];
               //             printf( "   chiSoCotBoLoc %d diaChiDeBoLoc %d  anh[diaChiDeBoLoc] %d\n", chiSoCotBoLoc, diaChiDeBoLoc, anh[diaChiDeBoLoc] );
               chiSoCotBoLoc++;
               diaChiDeBoLoc += 4;
            }
            
            anhBoLoc[diaChiAnh] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc[diaChiAnh+2] = giaTriLocDo/beRongBoLoc;
            anhBoLoc[diaChiAnh+3] = 0xff;
            diaChiAnh += 4;
            soCot++;
            
         }
         
         soHang++;
      }
      
      // ================ bộ lọc hượng dọc
      int cachMotHang = beRong << 2;   // số lượng byte giữa các hàng, cùng cột
      
      diaChiAnh = 0;
      soHang = 0;
      
      while( soHang < phanNuaBoLoc ) {
         //         printf( "soHang %d - ", soHang );
         unsigned short soCot = 0;
         int diaChiAnh = beRong*soHang << 2;
         while( soCot < beRong ) {
            unsigned char giaTriAnhCanhDuoi = anhBoLoc[soCot << 2];
            unsigned short soLuongDiemAnhNgoai = (phanNuaBoLoc - soHang);
            unsigned int giaTriLocXanh = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocLuc = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocDo = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            //           printf( " %d diaChiAnh %d  soLuongDiemAnhNgoai %d (%d)\n", soCot, diaChiAnh, soLuongDiemAnhNgoai,  beRongBoLoc - soLuongDiemAnhNgoai );
            // ---- chỉ số trong bộ lọc (phạm vi trong 0 tới beRongBoLoc)
            short chiSoHangBoLoc = 0;
            
            // ---- địa chỉ trong ảnh để bắt đầu bộ lọc
            int diaChiDeBoLoc = soCot << 2;
            
            while( chiSoHangBoLoc < beRongBoLoc - soLuongDiemAnhNgoai ) {
               giaTriLocDo += anhBoLoc[diaChiDeBoLoc];
               giaTriLocLuc += anhBoLoc[diaChiDeBoLoc+1];
               giaTriLocXanh += anhBoLoc[diaChiDeBoLoc+2];
               //               printf( "   chiSoHangBoLoc %d diaChiDeBoLoc %d\n", chiSoHangBoLoc, diaChiDeBoLoc );
               chiSoHangBoLoc++;
               diaChiDeBoLoc += cachMotHang;
            }
            
            anhBoLoc1[diaChiAnh] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc1[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc1[diaChiAnh+2] = giaTriLocDo/beRongBoLoc;
            anhBoLoc1[diaChiAnh+3] = 0xff;
            
            soCot++;
            diaChiAnh += 4;
         }
         
         soHang++;
      }
      //exit(0);
      soHangCuoi = beCao - phanNuaBoLoc;
      
      while( soHang < soHangCuoi ) {
         
         unsigned short soCot = 0;
         int diaChiAnh = (beRong*soHang + soCot) << 2;
         
         while( soCot < beRong ) {
            
            // ---- áp dụng bộ lọc
            unsigned int giaTriLocXanh = 0;
            unsigned int giaTriLocLuc = 0;
            unsigned int giaTriLocDo = 0;
            
            // ---- chỉ số trong bộ lọc
            short chiSoHangBoLoc = 0;
            // ---- cột tương đối với điểm ảnh
            short hangTuongDoi = -phanNuaBoLoc;
            // ---- tính địa chỉ đổ bộ lọc trong ảnh
            int diaChiDeBoLoc = diaChiAnh + (hangTuongDoi*beRong << 2);
            
            while( chiSoHangBoLoc < beRongBoLoc ) {
               giaTriLocDo += anhBoLoc[diaChiDeBoLoc];
               giaTriLocLuc += anhBoLoc[diaChiDeBoLoc+1];
               giaTriLocXanh += anhBoLoc[diaChiDeBoLoc+2];
               chiSoHangBoLoc++;
               diaChiDeBoLoc += cachMotHang;
            }
            
            anhBoLoc1[diaChiAnh] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc1[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc1[diaChiAnh+2] = giaTriLocDo/beRongBoLoc;
            anhBoLoc1[diaChiAnh+3] = 0xff;
            
            // ---- điểm ảnh tiếp
            soCot++;
            diaChiAnh += 4;
         };
         soHang++;
      }
      
      unsigned int diaChiHangCuoi = ((beCao - 1)*beRong << 2);
      while( soHang < beCao ) {
         
         unsigned short soCot = 0;
         int diaChiAnh = beRong*soHang << 2;
         while( soCot < beRong ) {
            unsigned char giaTriAnhCanhDuoi = anhBoLoc[diaChiHangCuoi];
            unsigned short soLuongDiemAnhNgoai = phanNuaBoLoc - (beCao - soHang - 1);
            unsigned int giaTriLocXanh = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocLuc = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocDo = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            // ---- chỉ số trong bộ lọc (phạm vi trong 0 tới beRongBoLoc)
            short chiSoHangBoLoc = 0;
            
            // ---- địa chỉ trong ảnh để bắt đầu bộ lọc
            short hangTuongDoi = -phanNuaBoLoc;
            int diaChiDeBoLoc = diaChiAnh + (hangTuongDoi*beRong << 2);
            
            while( chiSoHangBoLoc < beRongBoLoc - soLuongDiemAnhNgoai ) {
               giaTriLocDo += anhBoLoc[diaChiDeBoLoc];
               giaTriLocLuc += anhBoLoc[diaChiDeBoLoc+1];
               giaTriLocXanh += anhBoLoc[diaChiDeBoLoc+2];
               
               chiSoHangBoLoc++;
               diaChiDeBoLoc += cachMotHang;
            }
            
            anhBoLoc1[diaChiAnh] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc1[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc1[diaChiAnh+2] = giaTriLocDo/beRongBoLoc;
            anhBoLoc1[diaChiAnh+3] = 0xff;
            
            soCot++;
            diaChiAnh += 4;
         }
         
         soHang++;
      }
      
      free( anhBoLoc );
   }
   else {
      printf( "Sự khác: boLocTrungBinh: Vấn đệ tạo đệm cho sự khác biệt\n" );
   }
   printf( "\n" );
   return anhBoLoc1;
}

unsigned char *boLocTrungBinhDoc( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned short beRongBoLoc ) {
   
   // ---- mẫu số để "đơn vị hóa" giá trị từ bộ lọc (0 đến 255)
   unsigned char *anhBoLoc = malloc( beRong*beCao << 2 );
   unsigned short phanNuaBoLoc = beRongBoLoc >> 1;
   
   unsigned int chiSo = 0;
   unsigned int chiSoCuoi = beRong*beCao << 2;
   while( chiSo < chiSoCuoi ) {
      anhBoLoc[chiSo] = 0xff;
      chiSo++;
   }
   
   if( anhBoLoc ) {
      
      int cachMotHang = beRong << 2;   // số lượng byte giữa các hàng, cùng cột
      unsigned int diaChiAnh = 0;
      unsigned int soHang = 0;
      
      while( soHang < phanNuaBoLoc ) {
         
         unsigned short soCot = 0;
         int diaChiAnh = beRong*soHang << 2;
         while( soCot < beRong ) {
            unsigned char giaTriAnhCanhDuoi = anh[soCot << 2];
            unsigned short soLuongDiemAnhNgoai = (phanNuaBoLoc - soHang);
            unsigned int giaTriLocDo = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocLuc = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocXanh = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            
            // ---- chỉ số trong bộ lọc (phạm vi trong 0 tới beRongBoLoc)
            short chiSoHangBoLoc = 0;
            
            // ---- địa chỉ trong ảnh để bắt đầu bộ lọc
            int diaChiDeBoLoc = soCot << 2;
            
            while( chiSoHangBoLoc < beRongBoLoc - soLuongDiemAnhNgoai ) {
               giaTriLocDo += anh[diaChiDeBoLoc];
               giaTriLocLuc += anh[diaChiDeBoLoc+1];
               giaTriLocXanh += anh[diaChiDeBoLoc+2];
               //               if( soCot == 0 )
               //              printf( "%d  anh[diaChiDeBoLoc] %d   diaChiDeBoLoc %d   giaTriLocDo %d\n", chiSoHangBoLoc, anh[diaChiDeBoLoc], diaChiDeBoLoc, giaTriLocDo );
               
               chiSoHangBoLoc++;
               diaChiDeBoLoc += cachMotHang;
            }
            
            anhBoLoc[diaChiAnh] = giaTriLocDo/beRongBoLoc;
            anhBoLoc[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc[diaChiAnh+2] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc[diaChiAnh+3] = 0xff;
            
            soCot++;
            diaChiAnh += 4;
         }
         soHang++;
      }
      
      // ==== bộ lọc hượng dọc
      unsigned int soHangCuoi = beCao - phanNuaBoLoc;
      
      while( soHang < soHangCuoi ) {
         
         unsigned short soCot = 0;
         int diaChiAnh = (beRong*soHang + soCot) << 2;
         
         while( soCot < beRong ) {
            
            // ---- áp dụng bộ lọc
            unsigned int giaTriLocDo = 0;
            unsigned int giaTriLocLuc = 0;
            unsigned int giaTriLocXanh = 0;
            
            // ---- chỉ quét một dòng
            short chiSoHangBoLoc = 0;
            short hangTuongDoi = -phanNuaBoLoc;
            // ---- tính địa chỉ đổ bộ lọc trong ảnh
            int diaChiDeBoLoc = diaChiAnh + (hangTuongDoi*beRong << 2);
            
            while( chiSoHangBoLoc < beRongBoLoc ) {
               giaTriLocDo += anh[diaChiDeBoLoc];
               giaTriLocLuc += anh[diaChiDeBoLoc+1];
               giaTriLocXanh += anh[diaChiDeBoLoc+2];
               chiSoHangBoLoc++;
               diaChiDeBoLoc += cachMotHang;
            }
            
            anhBoLoc[diaChiAnh] = giaTriLocDo/beRongBoLoc;
            anhBoLoc[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc[diaChiAnh+2] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc[diaChiAnh+3] = 0xff;
            // ---- điểm ảnh tiếp
            soCot++;
            diaChiAnh += 4;
         };
         soHang++;
      }
      
      unsigned int diaChiHangCuoi = ((beCao - 1)*beRong << 2);
      while( soHang < beCao ) {
         
         unsigned short soCot = 0;
         int diaChiAnh = beRong*soHang << 2;
         while( soCot < beRong ) {
            unsigned char giaTriAnhCanhDuoi = anh[diaChiHangCuoi + (soCot << 2)];
            unsigned short soLuongDiemAnhNgoai = phanNuaBoLoc - (beCao - soHang - 1);
            unsigned int giaTriLocDo = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocLuc = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            unsigned int giaTriLocXanh = soLuongDiemAnhNgoai*giaTriAnhCanhDuoi;
            // ---- chỉ số trong bộ lọc (phạm vi trong 0 tới beRongBoLoc)
            short chiSoHangBoLoc = 0;
            
            // ---- địa chỉ trong ảnh để bắt đầu bộ lọc
            short hangTuongDoi = -phanNuaBoLoc;
            int diaChiDeBoLoc = diaChiAnh + (hangTuongDoi*beRong << 2);
            
            while( chiSoHangBoLoc < beRongBoLoc - soLuongDiemAnhNgoai ) {
               giaTriLocDo += anh[diaChiDeBoLoc];
               giaTriLocLuc += anh[diaChiDeBoLoc+1];
               giaTriLocXanh += anh[diaChiDeBoLoc+2];
               
               chiSoHangBoLoc++;
               diaChiDeBoLoc += cachMotHang;
            }
            
            anhBoLoc[diaChiAnh] = giaTriLocDo/beRongBoLoc;
            anhBoLoc[diaChiAnh+1] = giaTriLocLuc/beRongBoLoc;
            anhBoLoc[diaChiAnh+2] = giaTriLocXanh/beRongBoLoc;
            anhBoLoc[diaChiAnh+3] = 0xff;
            
            soCot++;
            diaChiAnh += 4;
         }
         
         soHang++;
      }
      
      
   }
   else {
      printf( "Sự khác: boLocTrungBinh: Vấn đệ tạo đệm cho sự khác biệt\n" );
   }
   printf( "\n" );
   return anhBoLoc;
}


#pragma mark ---- Giới Hạn
void gioiHan( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned char gioiHan ) {

   unsigned int diaChi = 0;
   unsigned int diaChiCuoi = beRong * beCao << 2;
   
   while( diaChi < diaChiCuoi ) {
      // ----
      if( anh[diaChi] < gioiHan )
         anh[diaChi] = 0;
      else
         anh[diaChi] = 255;
      // ----
      if( anh[diaChi+1] < gioiHan )
         anh[diaChi+1] = 0;
      else
         anh[diaChi+1] = 255;
      
      // ----
      if( anh[diaChi+2] < gioiHan )
         anh[diaChi+2] = 0;
      else
         anh[diaChi+2] = 255;

      diaChi += 4;
   }
}

#pragma mark ---- Tìm Mặt Na
ChuNhat tìmMatNa( unsigned char *anhMatNa, unsigned int beRong, unsigned int beCao, unsigned char gioiHan, unsigned short nuaBeRongBoLoc ) {

   // ---- quét dòng và tìm hàng các hàng có giá trị hơn giới hạn
   unsigned int soHang = nuaBeRongBoLoc;
   int batDauMatNa = -1;
   int ketThucMatNa = -1;
   
   while( soHang < beCao - nuaBeRongBoLoc) {
      
      unsigned int diaChiCot = soHang*beRong << 2;
      unsigned int soCot = nuaBeRongBoLoc;
      unsigned char hetHonGioiHan = kDUNG;  // BOOL, hết hơn giới hạn
      while( soCot < beRong - nuaBeRongBoLoc ) {
         if( anhMatNa[diaChiCot] < gioiHan ) {
            hetHonGioiHan = kSAI;
            soCot = beRong;  // bỏ cuộc và đi hàng tiếp
         }
         soCot++;
         diaChiCot += 4;
      }
      // ---- nếu hết điểm ảnh hơn giới hạn báo cáo
      if( hetHonGioiHan ) {
 //        printf( " hang tot: %d\n", soHang );
         if( batDauMatNa < 0 )
            batDauMatNa = soHang;
      }
      else {
         if( (batDauMatNa > -1) && (ketThucMatNa < 0) ) {
            ketThucMatNa = soHang;
         }
      }

      soHang++;
   }
   
   ChuNhat matNa;
   matNa.trai = nuaBeRongBoLoc;
   matNa.phai = beRong - nuaBeRongBoLoc;
   matNa.duoi = batDauMatNa;
   matNa.tren = ketThucMatNa;
   
   return matNa;
}

unsigned char *catAnh( unsigned char *anhGoc, unsigned int beRongGoc, unsigned int beCaoGoc, ChuNhat *matNa ) {
   
   unsigned int beRongCat = matNa->phai - matNa->trai;
   unsigned int beCaoCat = matNa->tren - matNa->duoi;
   unsigned char *anhCat = malloc(beRongCat*beCaoCat << 2);
   
   if( anhCat ) {
      unsigned int soHangGoc = matNa->duoi;
      unsigned int diaChiAnhCat = 0;
      while( soHangGoc < matNa->tren ) {

         unsigned int soCotGoc = matNa->trai;
         unsigned int diaChiAnhGoc = (soHangGoc*beRongGoc + soCotGoc) << 2;

         while( soCotGoc < matNa->phai ) {
            anhCat[diaChiAnhCat] = anhGoc[diaChiAnhGoc];
            anhCat[diaChiAnhCat+1] = anhGoc[diaChiAnhGoc+1];
            anhCat[diaChiAnhCat+2] = anhGoc[diaChiAnhGoc+2];
            anhCat[diaChiAnhCat+3] = anhGoc[diaChiAnhGoc+3];
            diaChiAnhGoc += 4;
            diaChiAnhCat += 4;
            soCotGoc++;
         }
         soHangGoc++;
      }
   }
   else {
      printf( "catAnh: vấn  ̣ề dành trí nhớ cho ảnh cắt\n" );
      return NULL;
   }
   
   return anhCat;
}

#pragma mark ---- Tìm Đạo Cao Và Thấp

unsigned char phanTichDiemCaoNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *diem );
unsigned char phanTichDiemThapNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *diem );
Diem diTheoDoiDuongVaChoDiemCuoi( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *diem, unsigned char cao );

Diem quetNghieng( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem tam, unsigned char cao );
Diem xemDiemQuetNghieng_cao( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem diem0, Diem diem1 );
Diem xemDiemQuetNghieng_thap( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem diem0, Diem diem1 );




void toMauCacDiem( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *mangDiemCao, unsigned short soLuongDiem, unsigned int mau );

// tìm điểm sáng nhất hơn điểm ảnh xung quang
//
// +------+------o------+------+
//              điểm
//
//    dốc trước        dốc
// |<----------->|<----------->|
unsigned char tìmDiemCaoNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned char cach, Net **mangNet ) {
   
   unsigned int soHang = 1140;  // Thử nghiệm, tìma các điểm cao trong ảnh và đi xuông theo nó

   // ---- TÌM KHỞI ĐẦU CHo CÁC ĐƯỜNG
   Diem mangDiemThichThu[64];  // <---- nên dùng malloc không?
   unsigned short chiSoMangToaDo = 0;
   
   unsigned int soCot = cach << 3;
   while( soCot < beRong - (cach << 1) ) {
      unsigned int diaChiAnh = (beRong*soHang + soCot) << 2;
      char docX_truoc = anh[diaChiAnh] - anh[diaChiAnh-(cach << 3)];
      char docX = anh[diaChiAnh+(cach << 2)] - anh[diaChiAnh-(cach << 2)];
      char docY = anh[diaChiAnh+(cach*beRong << 2)] - anh[diaChiAnh-(cach*beRong << 2)];
      float doc = sqrt( docX*docX + docY*docY );
//      printf( "  docX_truoc %d (%d) - (%d)   docX %d (%d) - (%d)\n", docX_truoc, diaChiAnh, diaChiAnh-(cach << 3), docX, diaChiAnh+(cach << 2), diaChiAnh-(cach << 2) );
      if( (docX_truoc >= 0) && (docX < 0) ) {

         mangDiemThichThu[chiSoMangToaDo].x = soCot;
         mangDiemThichThu[chiSoMangToaDo].y = soHang;
         mangDiemThichThu[chiSoMangToaDo].xuLyRoi = kSAI;
         if( phanTichDiemCaoNgang( anh, beRong, beCao, &(mangDiemThichThu[chiSoMangToaDo]) ) ) {
            printf( "----- %d %d\n", mangDiemThichThu[chiSoMangToaDo].x, mangDiemThichThu[chiSoMangToaDo].y );
            chiSoMangToaDo++;
         }
      }
//      printf("%d: (%3d; %3d)  (%d) %d\n", soCot, docX, docY, anh[diaChiAnh], diaChiAnh );
      soCot += cach;
      diaChiAnh += cach << 2;
   }

   printf( "số lượng điểm %d\n", chiSoMangToaDo );
   
   toMauCacDiem( anh, beRong, beCao, mangDiemThichThu, chiSoMangToaDo, 0x0000ffff );

   // ==== ĐI THEO DÕI ĐƯỜNG
   *mangNet = malloc( sizeof( Net )*chiSoMangToaDo );
   Diem mangDiemCuoiDung[64];  // <---- nên dùng malloc không?

   unsigned char soDiem = 0;
   while( soDiem < chiSoMangToaDo ) {
      
      if( !(mangDiemThichThu[soDiem].xuLyRoi) ) {
         // ---- kiếm các điểm ở dưới
         mangDiemThichThu[soDiem].huongX = 0.0f;
         mangDiemThichThu[soDiem].huongY = -1.0f;
 //       printf( " --- %d: diemThichThu %d %d\n", soDiem, mangDiemThichThu[soDiem].x, mangDiemThichThu[soDiem].y );
         
         // ---- cần giữ điểm để xem đường có kết nối với các điểm thích thú
         mangDiemCuoiDung[soDiem] = diTheoDoiDuongVaChoDiemCuoi( anh, beRong, beCao, &(mangDiemThichThu[soDiem]), kDUNG );
         
         // ---- xem có điểm thích thú để gần điểm cuối
         //      chỉ cần xem điểm thích thú sau, những điểm trước đã được xử lý rồi
         unsigned char chiSoDiemCuoi = 0;
         while( chiSoDiemCuoi < soDiem + 1 ) {
            unsigned char soDiemThichThuCon = soDiem + 1;
            while( soDiemThichThuCon < chiSoMangToaDo ) {
               // ---- tính các giữa điểm cuối và điểm thích thú
               int cachX = mangDiemThichThu[soDiemThichThuCon].x - mangDiemCuoiDung[chiSoDiemCuoi].x;
               int cachY = mangDiemThichThu[soDiemThichThuCon].y - mangDiemCuoiDung[chiSoDiemCuoi].y;
 //              printf( "mangDiemThichThu[%d] %d %d - mangDiemCuoiDung[%d] %d %d  cachX %d cachY %d ",
 //                     soDiemThichThuCon, mangDiemThichThu[soDiemThichThuCon].x, mangDiemThichThu[soDiemThichThuCon].y,
 //                     chiSoDiemCuoi, mangDiemCuoiDung[chiSoDiemCuoi].x, mangDiemCuoiDung[chiSoDiemCuoi].y,
 //                     cachX, cachY );
               unsigned int cachBinh = cachX*cachX + cachY*cachY;
   //            printf( " cachBinh %d\n", cachBinh );
               if( cachBinh < 1000 ) {
   //               printf( "  BO DIEM %d  tai %d; %d\n", soDiemThichThuCon,
  //                       mangDiemThichThu[soDiemThichThuCon].x, mangDiemThichThu[soDiemThichThuCon].y );
                  mangDiemThichThu[soDiemThichThuCon].xuLyRoi = kDUNG;
               }
               soDiemThichThuCon++;
            }
            chiSoDiemCuoi++;
         }
      }
      printf( "  soDeim %d  xu ly %d\n", soDiem, mangDiemThichThu[soDiem].xuLyRoi );
      soDiem++;
   }
   
   return chiSoMangToaDo;  // <----- KHÔNG ĐÚNG
}


// CHƯA SỬA LẠI ĐỂ KIẾK ĐIỂM THẤ
unsigned char tìmDiemThapNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned char cach, Net **mangNet ) {
   
   unsigned int soHang = 1140;  // Thử nghiệm, tìma các điểm cao trong ảnh và đi xuông theo nó
   
   // ---- TÌM KHỞI ĐẦU CHo CÁC ĐƯỜNG
   Diem mangDiemThichThu[64];  // <---- nên dùng malloc không?
   unsigned short chiSoMangToaDo = 0;
   
   unsigned int soCot = cach << 3;
   while( soCot < beRong - (cach << 1) ) {
      unsigned int diaChiAnh = (beRong*soHang + soCot) << 2;
      char docX_truoc = anh[diaChiAnh] - anh[diaChiAnh-(cach << 3)];
      char docX = anh[diaChiAnh+(cach << 2)] - anh[diaChiAnh-(cach << 2)];
      char docY = anh[diaChiAnh+(cach*beRong << 2)] - anh[diaChiAnh-(cach*beRong << 2)];
      float doc = sqrt( docX*docX + docY*docY );
      //      printf( "  docX_truoc %d (%d) - (%d)   docX %d (%d) - (%d)\n", docX_truoc, diaChiAnh, diaChiAnh-(cach << 3), docX, diaChiAnh+(cach << 2), diaChiAnh-(cach << 2) );
      if( (docX_truoc <= 0) && (docX > 0) ) {
         
         mangDiemThichThu[chiSoMangToaDo].x = soCot;
         mangDiemThichThu[chiSoMangToaDo].y = soHang;
         mangDiemThichThu[chiSoMangToaDo].xuLyRoi = kSAI;
         if( phanTichDiemThapNgang( anh, beRong, beCao, &(mangDiemThichThu[chiSoMangToaDo]) ) ) {
            printf( "----- %d %d\n", mangDiemThichThu[chiSoMangToaDo].x, mangDiemThichThu[chiSoMangToaDo].y );
            chiSoMangToaDo++;
         }
      }
      //      printf("%d: (%3d; %3d)  (%d) %d\n", soCot, docX, docY, anh[diaChiAnh], diaChiAnh );
      soCot += cach;
      diaChiAnh += cach << 2;
   }
   
   printf( "số lượng điểm %d\n", chiSoMangToaDo );
   
   toMauCacDiem( anh, beRong, beCao, mangDiemThichThu, chiSoMangToaDo, 0x00ff00ff );
   
   // ==== ĐI THEO DÕI ĐƯỜNG
   *mangNet = malloc( sizeof( Net)*chiSoMangToaDo );
   Diem mangDiemCuoiDung[64];  // <---- nên dùng malloc không?
   
   unsigned char soDiem = 0;
   while( soDiem < chiSoMangToaDo ) {
      
      if( !(mangDiemThichThu[soDiem].xuLyRoi) ) {
         // ---- kiếm các điểm ở dưới
         mangDiemThichThu[soDiem].huongX = 0.0f;
         mangDiemThichThu[soDiem].huongY = -1.0f;
         //       printf( " --- %d: diemThichThu %d %d\n", soDiem, mangDiemThichThu[soDiem].x, mangDiemThichThu[soDiem].y );
         
         // ---- cần giữ điểm để xem đường có kết nối với các điểm thích thú
         mangDiemCuoiDung[soDiem] = diTheoDoiDuongVaChoDiemCuoi( anh, beRong, beCao, &(mangDiemThichThu[soDiem]), kSAI );
         
         // ---- xem có điểm thích thú để gần điểm cuối
         //      chỉ cần xem điểm thích thú sau, những điểm trước đã được xử lý rồi
         unsigned char chiSoDiemCuoi = 0;
         while( chiSoDiemCuoi < soDiem + 1 ) {
            unsigned char soDiemThichThuCon = soDiem + 1;
            while( soDiemThichThuCon < chiSoMangToaDo ) {
               // ---- tính các giữa điểm cuối và điểm thích thú
               int cachX = mangDiemThichThu[soDiemThichThuCon].x - mangDiemCuoiDung[chiSoDiemCuoi].x;
               int cachY = mangDiemThichThu[soDiemThichThuCon].y - mangDiemCuoiDung[chiSoDiemCuoi].y;
               //              printf( "mangDiemThichThu[%d] %d %d - mangDiemCuoiDung[%d] %d %d  cachX %d cachY %d ",
               //                     soDiemThichThuCon, mangDiemThichThu[soDiemThichThuCon].x, mangDiemThichThu[soDiemThichThuCon].y,
               //                     chiSoDiemCuoi, mangDiemCuoiDung[chiSoDiemCuoi].x, mangDiemCuoiDung[chiSoDiemCuoi].y,
               //                     cachX, cachY );
               unsigned int cachBinh = cachX*cachX + cachY*cachY;
               //            printf( " cachBinh %d\n", cachBinh );
               if( cachBinh < 1000 ) {
                  //               printf( "  BO DIEM %d  tai %d; %d\n", soDiemThichThuCon,
                  //                       mangDiemThichThu[soDiemThichThuCon].x, mangDiemThichThu[soDiemThichThuCon].y );
                  mangDiemThichThu[soDiemThichThuCon].xuLyRoi = kDUNG;
               }
               soDiemThichThuCon++;
            }
            chiSoDiemCuoi++;
         }
      }
      printf( "  soDeim %d  xu ly %d\n", soDiem, mangDiemThichThu[soDiem].xuLyRoi );
      soDiem++;
   }
   
   return chiSoMangToaDo;  // <----- KHÔNG ĐÚNG
}


// - quét tới và lui kiếm điểm cao nhất  <----+---->
// - xem có điểm này trong danh sách điểm hay chưa
//      chưa có: bỏ vào danh sách
//      có rồi: bỏ nỏ


#define kCACH_KIEM_TRA_DIEM_CAO 40  // điểm ảnh

// vài trường hợp:
//     1.          -*----
//         -------/      \------   <---- tìm điểm cao
//
//     2.          ------
//         ---*---/      \------   <---- tìm điểm cao - CHƯA được kiếm trường hợp này
//
//     3.          ------
//         -------/      \---*--   <---- tìm điểm cao - CHƯA được kiếm trường hợp này
//
//     4.             ----------   <---- bỏ trường hợp này
//         ------*---/
//
//     5.             -----*----   <---- bỏ trường hợp này
//         ----------/

#define kCHUA_BIET 0
#define kDOC_XUONG 1
#define kDOC_LEN   2

unsigned char phanTichDiemCaoNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *diem ) {

   // ---- vị trí điểm đang phân tích
   unsigned int x = diem->x;
   unsigned int y = diem->y;
   
   // ---- địa chỉ của điểm trong ảnh
   unsigned int diaChiAnh = (beRong*y + x) << 2;
   // ---- giá trị cao từ điểm ảnh, đây là giá trị khở đầu đê so sánh
   unsigned char giaTriCao = anh[diaChiAnh];
   
   // ---- xem điểm phía sau (-x)
   unsigned char trangThaiLui = kCHUA_BIET;
   
   unsigned char giaTriHienTai = anh[diaChiAnh - (kCACH_KIEM_TRA_DIEM_CAO << 2)];
//   printf( " (%d; %d )\n lui  giaTriHienTai %d\n", x, y, giaTriHienTai );
//   printf( " cao %d\n", giaTriCao );
   if( giaTriHienTai < giaTriCao )
      trangThaiLui = kDOC_LEN;
   else if( giaTriHienTai > giaTriCao )
      trangThaiLui = kDOC_XUONG;
   
   // ---- xem điểm phía trước (+x)
   unsigned char trangThaiToi = kCHUA_BIET;
   giaTriHienTai = anh[diaChiAnh + (kCACH_KIEM_TRA_DIEM_CAO << 2)];
//   printf( " toi  giaTriHienTai %d\n", giaTriHienTai );
   
   if( giaTriHienTai < giaTriCao )
      trangThaiToi = kDOC_XUONG;
   else if( giaTriHienTai > giaTriCao )
      trangThaiToi = kDOC_LEN;
   
   // ---- xem kết qủa
   if( (trangThaiLui == kDOC_LEN) && (trangThaiToi == kDOC_XUONG) )
      return kDUNG;
   
   return kSAI;
}

unsigned char phanTichDiemThapNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *diem ) {
   
   // ---- vị trí điểm đang phân tích
   unsigned int x = diem->x;
   unsigned int y = diem->y;
   
   // ---- địa chỉ của điểm trong ảnh
   unsigned int diaChiAnh = (beRong*y + x) << 2;
   // ---- giá trị thấp từ điểm ảnh, đây là giá trị khở đầu đê so sánh
   unsigned char giaTriThap = anh[diaChiAnh];
   
   // ---- xem điểm phía sau (-x)
   unsigned char trangThaiLui = kCHUA_BIET;
   
   unsigned char giaTriHienTai = anh[diaChiAnh - (kCACH_KIEM_TRA_DIEM_CAO << 2)];
   //   printf( " (%d; %d )\n lui  giaTriHienTai %d\n", x, y, giaTriHienTai );
   //   printf( " thap %d\n", giaTriCao );
   if( giaTriHienTai > giaTriThap )
      trangThaiLui = kDOC_LEN;
   else if( giaTriHienTai < giaTriThap )
      trangThaiLui = kDOC_XUONG;
   
   // ---- xem điểm phía trước (+x)
   unsigned char trangThaiToi = kCHUA_BIET;
   giaTriHienTai = anh[diaChiAnh + (kCACH_KIEM_TRA_DIEM_CAO << 2)];
   //   printf( " toi  giaTriHienTai %d\n", giaTriHienTai );
   
   if( giaTriHienTai > giaTriThap )
      trangThaiToi = kDOC_XUONG;
   else if( giaTriHienTai < giaTriThap )
      trangThaiToi = kDOC_LEN;
   
   // ---- xem kết qủa
   if( (trangThaiLui == kDOC_LEN) && (trangThaiToi == kDOC_XUONG) )
      return kDUNG;
   
   return kSAI;
}


Diem diTheoDoiDuongVaChoDiemCuoi( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *diem, unsigned char cao ) {
   
   unsigned int gioiHanThap = 200;// <---- NHỚ BỎ CÁI NÀY KHI DÙNG MẶT NẠ
   unsigned int gioiHanCao = 1150;// <---- NHỚ BỎ CÁI NÀY KHI DÙNG MẶT NẠ

   Diem danhSachDiem[1024];  // chứa điểm
   danhSachDiem[0] = *diem;

   Diem nganXepDiem[16];  // chứa điểm đang kiểm tra
   
   // ---- tiến tới
   Diem diem0;
   diem0.x = diem->x;
   diem0.y = diem->y;
   diem0.huongX = diem->huongX;
   diem0.huongY = diem->huongY;
   
   unsigned short dem = 0;    // đừng cho điTheoĐường() bị mất kế
   
   while( (diem0.y > gioiHanThap) && (diem0.y < gioiHanCao) ) {

      Diem diemMoi = quetNghieng( anh, beRong, beCao, diem0, cao );
  //    printf( " diTheoDoiDuong: truoc  diem0: %d %d   diemMoi: %d  %d  huong %5.3f %5.3f\n", diem0.x, diem0.y, diemMoi.x, diemMoi.y, diemMoi.huongX, diemMoi.huongY );
      // ---- đừng cho điểm ra ngoài phạm vi, dùng hướng của điểm để sửa lại vị trí năm trên ranh giới hạn
      if( diemMoi.y > gioiHanCao ) {
         float soPhan = (float)(gioiHanCao - diem0.y)/(float)(diemMoi.y - diem0.y);
         diemMoi.x = diem0.x + (diemMoi.x - diem0.x)*soPhan;
         diemMoi.y = diem0.y + (diemMoi.y - diem0.y)*soPhan;
         printf( " diTheoDoiDuong: sau diemMoi: %d  %d  soPhan %5.3f  gioiHanThap %d\n", diemMoi.x, diemMoi.y, soPhan, gioiHanThap );
      }
      else if( diemMoi.y < gioiHanThap ) {
         float soPhan = (float)(diem0.y - gioiHanThap)/(float)(diem0.y - diemMoi.y);
         diemMoi.x = diem0.x + (diemMoi.x - diem0.x)*soPhan;
         diemMoi.y = diem0.y + (diemMoi.y - diem0.y)*soPhan;
         printf( " diTheoDoiDuong: sau diemMoi: %d  %d  soPhan %5.3f  gioiHanThap %d\n", diemMoi.x, diemMoi.y, soPhan, gioiHanThap );
 //        exit(0);
      }

      printf("  %d  %d %d  huong di %5.3f %5.3f  doc %d  cao %d\n", dem, diemMoi.x, diemMoi.y, diemMoi.huongX, diemMoi.huongY, diemMoi.doSang - diem0.doSang, cao );

      unsigned int mau;
      if( cao )
         mau = (dem << 26) | 0xffff;  // tô màu xanh-tím
      else
         mau = (dem << 26) | 0xff00ff;  // tô màu lục

      toMauCacDiem( anh, beRong, beCao, &diemMoi, 1, mau );
      veDuong( anh, beRong, beCao, diem0, diemMoi, mau );
      
      diem0 = diemMoi;
      dem++;
      if( dem > 45 )
         break;
   }
//   exit(0);
   // ---- xem đã đến từ hướng này chưa
   return diem0;
}


#define kCACH_QUET 50
Diem quetNghieng( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem tam, unsigned char cao ) {
   
   float huongVuongGocX = tam.huongY;
   float huongVuongGocY = -tam.huongX;
   
   // ---- tính điểm khởi đầu và kết thúc
   Diem diemBatDau;
   Diem diemKetThuc;
   diemBatDau.x = tam.x + tam.huongX*50.0f - huongVuongGocX*kCACH_QUET;
   diemBatDau.y = tam.y + tam.huongY*50.0f - huongVuongGocY*kCACH_QUET;
   diemKetThuc.x = tam.x + tam.huongX*50.0f + huongVuongGocX*kCACH_QUET;
   diemKetThuc.y = tam.y + tam.huongY*50.0f + huongVuongGocY*kCACH_QUET;
//   printf( "  tam %d %d  huong %5.3f %5.3f  huongVG %5.3f %5.3f\n   batdau (%d; %d)  ketThuc (%d; %d)\n", tam.x, tam.y, tam.huongX, tam.huongY, huongVuongGocX, huongVuongGocY,  diemBatDau.x, diemBatDau.y, diemKetThuc.x, diemKetThuc.y );
   
   Diem diemMoi;
   if( cao )
      diemMoi = xemDiemQuetNghieng_cao( anh, beRong, beCao, diemBatDau, diemKetThuc );
   else
      diemMoi = xemDiemQuetNghieng_thap( anh, beRong, beCao, diemBatDau, diemKetThuc );
 
   short huongX = diemMoi.x - tam.x;
   short huongY = diemMoi.y - tam.y;
//   printf( " diemMoi %d %d  tam %d %d huongX %d  huongY %d\n", diemMoi.x, diemMoi.y, tam.x, tam.y, huongX, huongY  );
   float doLon = sqrtf( huongX*huongX + huongY*huongY );
   diemMoi.huongX = huongX/doLon;
   diemMoi.huongY = huongY/doLon;
   
   return diemMoi;
}


Diem xemDiemQuetNghieng_cao( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem diem0, Diem diem1 ) {

   short cachX = diem1.x - diem0.x;
   short cachY = diem1.y - diem0.y;
   Diem mangDiemQuet[(kCACH_QUET << 1) + 1];
   unsigned char giaTriCao = 0;
   unsigned short soLan = 0;
   unsigned short chiSoCaoNhat = 0;
   //   printf( " cach %d %d\n", cachX, cachY );
   
   // ---- ∆x < 0; ∆y < 0 đổi thành ∆x > 0; ∆y > 0
   if( (cachX <= 0) && (cachY <= 0) ) {
      //     printf( " ...dang trao doi\n");
      cachX = -cachX;
      cachY = -cachY;
      short so_x = diem0.x;
      short so_y = diem0.y;
      diem0.x = diem1.x;
      diem0.y = diem1.y;
      diem1.x = so_x;
      diem1.y = so_y;
   }
   else if( (cachX > 0) && (cachY < 0) ) {
      cachX = -cachX;
      cachY = -cachY;
      short so_x = diem0.x;
      short so_y = diem0.y;
      diem0.x = diem1.x;
      diem0.y = diem1.y;
      diem1.x = so_x;
      diem1.y = so_y;
   }
   //   printf( " (sau trao doi) cach %d %d  diem0 %d %d   diem1 %d %d\n", cachX, cachY, diem0.x, diem0.y, diem1.x, diem1.y );
   
   unsigned short chiSoMang = 0;
   short x = diem0.x;
   short y = diem0.y;
   
   if( (cachX == 0) && (cachY != 0) ) {  // đường dọc
      while( y <= diem1.y ) {
         unsigned int diaChiAnh = (beRong*y + x) << 2;
         mangDiemQuet[chiSoMang].x = x;
         mangDiemQuet[chiSoMang].y = y;
         unsigned char giaTriHienTai = anh[diaChiAnh];
         mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
         if( giaTriHienTai > giaTriCao ) {
            giaTriCao = giaTriHienTai;
            soLan = 1;
            chiSoCaoNhat = chiSoMang;
         }
         else if( giaTriHienTai == giaTriCao ) {
            soLan++;
         }
//         printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
         y++;
         chiSoMang++;
      }
   }
   else if( (cachX != 0) && (cachY == 0) ) {  // đường ngang
      while( x <= diem1.x ) {
         unsigned int diaChiAnh = (beRong*y + x) << 2;
         mangDiemQuet[chiSoMang].x = x;
         mangDiemQuet[chiSoMang].y = y;
         unsigned char giaTriHienTai = anh[diaChiAnh];
         mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
         if( giaTriHienTai > giaTriCao ) {
            giaTriCao = giaTriHienTai;
            soLan = 1;
            chiSoCaoNhat = chiSoMang;
         }
         else if( giaTriHienTai == giaTriCao ) {
            soLan++;
         }
//         printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
         x++;
         chiSoMang++;
      }
   }
   
   else {
      if( cachX > 0 && cachY > 0 ) {  // ---- ∆x > 0; ∆y > 0
         if( cachY <= cachX ) {
            short saiLam = (cachY << 1) - cachX;
            
            while( x <= diem1.x ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai > giaTriCao ) {
                  giaTriCao = giaTriHienTai;
                  soLan = 1;
                  chiSoCaoNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriCao ) {
                  soLan++;
               }
//               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );

               if( saiLam > 0 ) {
                  y++;
                  saiLam -= (cachX << 1);
               }
               saiLam += cachY << 1;
               chiSoMang++;
               x++;
            }
         }
         else {
            short saiLam = (cachX << 1) - cachY;
            
            while( y <= diem1.y ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai > giaTriCao ) {
                  giaTriCao = giaTriHienTai;
                  soLan = 1;
                  chiSoCaoNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriCao ) {
                  soLan++;
               }
//               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );

               if( saiLam > 0 ) {
                  x++;
                  saiLam -= (cachY << 1);
               }
               saiLam += cachX << 1;
               chiSoMang++;
               y++;
            }
         }
      }
      else {           // ---- ∆x < 0; ∆y > 0
         short cachX_tuyetDoi = -cachX;
         if( cachY <= cachX_tuyetDoi ) {
            short saiLam = (cachY << 1) + cachX;
            
            while( x >= diem1.x ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai > giaTriCao ) {
                  giaTriCao = giaTriHienTai;
                  soLan = 1;
                  chiSoCaoNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriCao ) {
                  soLan++;
               }
//               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );

               if( saiLam > 0 ) {
                  y++;
                  saiLam += (cachX << 1);
               }
               saiLam += cachY << 1;
               chiSoMang++;
               x--;
            }
         }
         else {
            short saiLam = -(cachX << 1) - cachY;
            
            while( y <= diem1.y ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai > giaTriCao ) {
                  giaTriCao = giaTriHienTai;
                  soLan = 1;
                  chiSoCaoNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriCao ) {
                  soLan++;
               }
//               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );

               if( saiLam > 0 ) {
                  x--;
                  saiLam -= (cachY << 1);
               }
               saiLam -= cachX << 1;
               chiSoMang++;
               y++;
               
            }
            
         }
      }
   }

   return mangDiemQuet[chiSoCaoNhat + (soLan >> 1)];
}

Diem xemDiemQuetNghieng_thap( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem diem0, Diem diem1 ) {
   
   short cachX = diem1.x - diem0.x;
   short cachY = diem1.y - diem0.y;
   Diem mangDiemQuet[(kCACH_QUET << 1) + 1];
   unsigned char giaTriThap = 255;
   unsigned short soLan = 0;
   unsigned short chiSoThapNhat = 0;
   //   printf( " cach %d %d\n", cachX, cachY );
   
   // ---- ∆x < 0; ∆y < 0 đổi thành ∆x > 0; ∆y > 0
   if( (cachX <= 0) && (cachY <= 0) ) {
      //     printf( " ...dang trao doi\n");
      cachX = -cachX;
      cachY = -cachY;
      short so_x = diem0.x;
      short so_y = diem0.y;
      diem0.x = diem1.x;
      diem0.y = diem1.y;
      diem1.x = so_x;
      diem1.y = so_y;
   }
   else if( (cachX > 0) && (cachY < 0) ) {
      cachX = -cachX;
      cachY = -cachY;
      short so_x = diem0.x;
      short so_y = diem0.y;
      diem0.x = diem1.x;
      diem0.y = diem1.y;
      diem1.x = so_x;
      diem1.y = so_y;
   }
   //   printf( " (sau trao doi) cach %d %d  diem0 %d %d   diem1 %d %d\n", cachX, cachY, diem0.x, diem0.y, diem1.x, diem1.y );
   
   unsigned short chiSoMang = 0;
   short x = diem0.x;
   short y = diem0.y;
   
   if( (cachX == 0) && (cachY != 0) ) {  // đường dọc
      while( y <= diem1.y ) {
         unsigned int diaChiAnh = (beRong*y + x) << 2;
         mangDiemQuet[chiSoMang].x = x;
         mangDiemQuet[chiSoMang].y = y;
         unsigned char giaTriHienTai = anh[diaChiAnh];
         mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
         if( giaTriHienTai < giaTriThap ) {
            giaTriThap = giaTriHienTai;
            soLan = 1;
            chiSoThapNhat = chiSoMang;
         }
         else if( giaTriHienTai == giaTriThap ) {
            soLan++;
         }
         //         printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
         y++;
         chiSoMang++;
      }
   }
   else if( (cachX != 0) && (cachY == 0) ) {  // đường ngang
      while( x <= diem1.x ) {
         unsigned int diaChiAnh = (beRong*y + x) << 2;
         mangDiemQuet[chiSoMang].x = x;
         mangDiemQuet[chiSoMang].y = y;
         unsigned char giaTriHienTai = anh[diaChiAnh];
         mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
         if( giaTriHienTai < giaTriThap ) {
            giaTriThap = giaTriHienTai;
            soLan = 1;
            chiSoThapNhat = chiSoMang;
         }
         else if( giaTriHienTai == giaTriThap ) {
            soLan++;
         }
         //         printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
         x++;
         chiSoMang++;
      }
   }
   
   else {
      if( cachX > 0 && cachY > 0 ) {  // ---- ∆x > 0; ∆y > 0
         if( cachY <= cachX ) {
            short saiLam = (cachY << 1) - cachX;
            
            while( x <= diem1.x ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai < giaTriThap ) {
                  giaTriThap = giaTriHienTai;
                  soLan = 1;
                  chiSoThapNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriThap ) {
                  soLan++;
               }
               //               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
               
               if( saiLam > 0 ) {
                  y++;
                  saiLam -= (cachX << 1);
               }
               saiLam += cachY << 1;
               chiSoMang++;
               x++;
            }
         }
         else {
            short saiLam = (cachX << 1) - cachY;
            
            while( y <= diem1.y ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai < giaTriThap ) {
                  giaTriThap = giaTriHienTai;
                  soLan = 1;
                  chiSoThapNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriThap ) {
                  soLan++;
               }
               //               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
               
               if( saiLam > 0 ) {
                  x++;
                  saiLam -= (cachY << 1);
               }
               saiLam += cachX << 1;
               chiSoMang++;
               y++;
            }
         }
      }
      else {           // ---- ∆x < 0; ∆y > 0
         short cachX_tuyetDoi = -cachX;
         if( cachY <= cachX_tuyetDoi ) {
            short saiLam = (cachY << 1) + cachX;
            
            while( x >= diem1.x ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai < giaTriThap ) {
                  giaTriThap = giaTriHienTai;
                  soLan = 1;
                  chiSoThapNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriThap ) {
                  soLan++;
               }
               //               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
               
               if( saiLam > 0 ) {
                  y++;
                  saiLam += (cachX << 1);
               }
               saiLam += cachY << 1;
               chiSoMang++;
               x--;
            }
         }
         else {
            short saiLam = -(cachX << 1) - cachY;
            
            while( y <= diem1.y ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               mangDiemQuet[chiSoMang].x = x;
               mangDiemQuet[chiSoMang].y = y;
               unsigned char giaTriHienTai = anh[diaChiAnh];
               mangDiemQuet[chiSoMang].doSang = giaTriHienTai;
               if( giaTriHienTai < giaTriThap ) {
                  giaTriThap = giaTriHienTai;
                  soLan = 1;
                  chiSoThapNhat = chiSoMang;
               }
               else if( giaTriHienTai == giaTriThap ) {
                  soLan++;
               }
               //               printf( "%d: %d %d  giaTri %d  diaChiAnh %d\n", chiSoMang, x, y, mangDiemQuet[chiSoMang].doSang, diaChiAnh );
               
               if( saiLam > 0 ) {
                  x--;
                  saiLam -= (cachY << 1);
               }
               saiLam -= cachX << 1;
               chiSoMang++;
               y++;
               
            }
            
         }
      }
   }
   
   return mangDiemQuet[chiSoThapNhat + (soLan >> 1)];
}


void toMauCacDiem( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem *mangDiemCao, unsigned short soLuongDiem, unsigned int mau ) {

   // ---- các màu
   unsigned char mauDo = mau >> 24;
   unsigned char mauLuc = mau >> 16;
   unsigned char mauXanh = mau >> 8;
   
   unsigned short chiSo = 0;
   while( chiSo < soLuongDiem ) {
      int x = mangDiemCao[chiSo].x;
      int y = mangDiemCao[chiSo].y;
      
      int diaChiAnh = (beRong*y + x) << 2;

      // ---- tô hàng giữa
      if( x > 0 ) {
         anh[diaChiAnh - 4] = mauDo;
         anh[diaChiAnh - 3] = mauLuc;
         anh[diaChiAnh - 2] = mauXanh;
      }
      
      anh[diaChiAnh] = mauDo;
      anh[diaChiAnh+1] = mauLuc;
      anh[diaChiAnh+2] = mauXanh;
      
      if( x < beRong - 1 ) {
         anh[diaChiAnh + 4] = mauDo;
         anh[diaChiAnh + 5] = mauLuc;
         anh[diaChiAnh + 6] = mauXanh;
      }
      
      // ---- tô hàng dưới
      if( y > 0 ) {
         if( x > 0 ) {
            diaChiAnh -= beRong<<2;
            anh[diaChiAnh - 4] = mauDo;
            anh[diaChiAnh - 3] = mauLuc;
            anh[diaChiAnh - 2] = mauXanh;
         }
         
         anh[diaChiAnh] = mauDo;
         anh[diaChiAnh + 1] = mauLuc;
         anh[diaChiAnh + 2] = mauXanh;
         
         if( (x < beRong - 1) ) {
            anh[diaChiAnh + 4] = mauDo;
            anh[diaChiAnh + 5] = mauLuc;
            anh[diaChiAnh + 6] = mauXanh;
         }
      }
      
      // ---- tô hàng trên
      if( y < beCao - 1 ) {
         
         diaChiAnh += beRong<<3;
         if( x > 0 ) {
            anh[diaChiAnh - 4] = mauDo;
            anh[diaChiAnh - 3] = mauLuc;
            anh[diaChiAnh - 2] = mauXanh;
         }
         
         anh[diaChiAnh] = mauDo;
         anh[diaChiAnh + 1] = mauLuc;
         anh[diaChiAnh + 2] = mauXanh;
         
         if( x < beRong - 1 ) {
            anh[diaChiAnh + 4] = mauDo;
            anh[diaChiAnh + 5] = mauLuc;
            anh[diaChiAnh + 6] = mauXanh;
         }
      }
      chiSo++;
   }
}


// Giải thuật Bresenham
void veDuong( unsigned char *anh, unsigned int beRong, unsigned int beCao, Diem diem0, Diem diem1, unsigned int mau ) {
   
   unsigned char mauDo = mau >> 24;
   unsigned char mauLuc = mau >> 16;
   unsigned char mauXanh = mau >> 8;

   short cachX = diem1.x - diem0.x;
   short cachY = diem1.y - diem0.y;
//   printf( " cach %d %d\n", cachX, cachY );
   
   // ---- ∆x < 0; ∆y < 0 đổi thành ∆x > 0; ∆y > 0
   if( (cachX <= 0) && (cachY <= 0) ) {
 //     printf( " ...dang trao doi\n");
      cachX = -cachX;
      cachY = -cachY;
      short so_x = diem0.x;
      short so_y = diem0.y;
      diem0.x = diem1.x;
      diem0.y = diem1.y;
      diem1.x = so_x;
      diem1.y = so_y;
   }
   else if( (cachX > 0) && (cachY < 0) ) {
      cachX = -cachX;
      cachY = -cachY;
      short so_x = diem0.x;
      short so_y = diem0.y;
      diem0.x = diem1.x;
      diem0.y = diem1.y;
      diem1.x = so_x;
      diem1.y = so_y;
   }
//   printf( " (sau trao doi) cach %d %d  diem0 %d %d   diem1 %d %d\n", cachX, cachY, diem0.x, diem0.y, diem1.x, diem1.y );

   short x = diem0.x;
   short y = diem0.y;
   
   if( (cachX == 0) && (cachY != 0) ) {  // đường dọc
      while( y <= diem1.y ) {
         unsigned int diaChiAnh = (beRong*y + x) << 2;
         anh[diaChiAnh] = mauDo;
         anh[diaChiAnh+1] = mauLuc;
         anh[diaChiAnh+2] = mauXanh;
         y++;
      }
   }
   else if( (cachX != 0) && (cachY == 0) ) {  // đường ngang
      while( x <= diem1.x ) {
         unsigned int diaChiAnh = (beRong*y + x) << 2;
         anh[diaChiAnh] = mauDo;
         anh[diaChiAnh+1] = mauLuc;
         anh[diaChiAnh+2] = mauXanh;
         x++;
      }
   }
   
   else {
      if( cachX > 0 && cachY > 0 ) {  // ---- ∆x > 0; ∆y > 0
         if( cachY <= cachX ) {
            short saiLam = (cachY << 1) - cachX;
            
            while( x <= diem1.x ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               anh[diaChiAnh] = mauDo;
               anh[diaChiAnh+1] = mauLuc;
               anh[diaChiAnh+2] = mauXanh;
               
               if( saiLam > 0 ) {
                  y++;
                  saiLam -= (cachX << 1);
               }
               saiLam += cachY << 1;
               
               x++;
            }
         }
         else {
            short saiLam = (cachX << 1) - cachY;
            
            while( y <= diem1.y ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               anh[diaChiAnh] = mauDo;
               anh[diaChiAnh+1] = mauLuc;
               anh[diaChiAnh+2] = mauXanh;
               
               if( saiLam > 0 ) {
                  x++;
                  saiLam -= (cachY << 1);
               }
               saiLam += cachX << 1;
               
               y++;
            }
         }
      }
      else {           // ---- ∆x < 0; ∆y > 0
         short cachX_tuyetDoi = -cachX;
         if( cachY <= cachX_tuyetDoi ) {
            short saiLam = (cachY << 1) + cachX;
            
            while( x >= diem1.x ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               anh[diaChiAnh] = mauDo;
               anh[diaChiAnh+1] = mauLuc;
               anh[diaChiAnh+2] = mauXanh;

               if( saiLam > 0 ) {
                  y++;
                  saiLam += (cachX << 1);
               }
               saiLam += cachY << 1;
               
               x--;
            }
         }
         else {
            short saiLam = -(cachX << 1) - cachY;
            
            while( y <= diem1.y ) {
               unsigned int diaChiAnh = (beRong*y + x) << 2;
               anh[diaChiAnh] = mauDo;
               anh[diaChiAnh+1] = mauLuc;
               anh[diaChiAnh+2] = mauXanh;
               
               if( saiLam > 0 ) {
                  x--;
                  saiLam -= (cachY << 1);
               }
               saiLam -= cachX << 1;
               
               y++;
       
            }
       
         }
      }
      
   }
}

#pragma mark ---- Lật Ngược
void lậtNgược( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {
   
   unsigned int diaChi = 0;
   unsigned int diaChiCuoi = beRong * beCao << 2;
   
   while( diaChi < diaChiCuoi ) {
      anh[diaChi] = 255 - anh[diaChi];
      anh[diaChi+1] = 255 - anh[diaChi+1];
      anh[diaChi+2] = 255 - anh[diaChi+2];
      diaChi += 4;
   }
}


#pragma mark ---- Tô Màu
/* Tìm Đường */
unsigned char tìmDiemCaoNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned char cach, Net **mangNet );
unsigned char tìmDiemThapNgang( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned char cach, Net **mangNet );


unsigned char danhSachMau[] = {
   0x00, 0x00, 0x7f, 0xff,
   0x00, 0x00, 0xff, 0xff,
   0x00, 0x7f, 0xff, 0xff,
   0x00, 0xff, 0xff, 0xff,
   0x00, 0xff, 0x7f, 0xff,
   0x00, 0xff, 0x00, 0xff,
   0x7f, 0xff, 0x00, 0xff,
   0xff, 0xff, 0x00, 0xff,  //
};

float danhSachGiTri[] = {
   000.0f, 100.0f, 200.0f, 300.0f, 400.0f, 500.0f, 600.0f, 700.0f,
};

unsigned char *toMauAnh( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {
   
   unsigned char *anhToMau = malloc( beRong * beCao << 2 );
   
   if( anhToMau ) {
      Net *mangNetCao;
      Net *mangNetThap;
      
      unsigned char soLuongNetCao = tìmDiemCaoNgang( anh, beRong, beCao, 10, &mangNetCao );
      unsigned char soLuongNetThap = tìmDiemThapNgang( anh, beRong, beCao, 10, &mangNetThap );
      
   }
   else {
      printf( "TôMàuẢnh: vấn đề tạo tô màu\n" );
   }
   
   return anhToMau;
}

#pragma mark ==== Tệp PNG
// ---- thứ dữ liệu màu trong tập tin
//#define kMAU_XAM        0x00  // màu xám
//#define kBANG_MAU_XAM  0x01  // có bảng màu xám
//#define kRGB            0x02  // màu đỏ, lục, xanh
//#define kBANG_MAU       0x03  // có bảng màu đỏ, lục, xanh
//#define kMAU_XAM_DUC    0x04  // màu xám, đục
//#define kBANG_MAU_XAM_DUC 0x05  // có bảng màu xám đục
#define kRGBO           0x06  // màu đỏ, lục, xanh, đục <------- chỉ xài cái này
//#define kBANG_MAU_DUC     0x07  // có bảng màu đỏ, lục, xanh, đục


// ---- LƯU Ý: đã đặt thứ tự byte ngược cho Intel
#define kTHANH_PHAN__LOAI_IHDR 0x49484452
#define kTHANH_PHAN__LOAI_CgBI 0x43674249  // không biết, đặc biệt cho iOS
#define kTHANH_PHAN__LOAI_IDAT 0x49444154

#define kTHANH_PHAN__LOAI_pHYs 0x70485973
#define kTHANH_PHAN__LOAI_sBIT 0x73424954
#define kTHANH_PHAN__LOAI_tEXt 0x74455874
#define kTHANH_PHAN__LOAI_bKGD 0x624B4744
#define kTHANH_PHAN__LOAI_sRGB 0x73524742
#define kTHANH_PHAN__LOAI_tIME 0x74494D45

#define kTHANH_PHAN__LOAI_IEND 0x49454E44

#define kCO_THUOC_TOI_DA_IDAT  8192   // cờ thước tối đa khi chẻ thành phần IDAT
#define kKHO_ANH_TOI_DA 8192   // khổ điểm ảnh lớn tối đa
#define kZLIB_MUC_NEN 6

// ---- kèm thành phần
void kemThanhPhanIHDRChoDong( FILE *dongTapTin, unsigned int beRong, unsigned int beCao, unsigned char loai, unsigned char bitMoiKenh );
void kemThanhPhanTIMEChoDong( FILE *dongTapTin, unsigned short nam, unsigned char thang, unsigned char ngay, unsigned char gio, unsigned char phut, unsigned char giay );
void kemThanhPhanIENDChoDong( FILE *dongTapTin );
void kemThanhPhanIDATChoDong( FILE *dongTapTin, unsigned char *duLieuMauAnhNen, unsigned int beDaiDuLieuNen );

// ---- bộ lọc
unsigned char *locDuLieuAnh_32bit( unsigned char *duLieuAnh, unsigned short beRong, unsigned short beCao, unsigned int *beDaiDuLieuAnhLoc);

// ---- CRC
unsigned long nang_cap_crc(unsigned long crc, unsigned char *buf, int len);
void tao_bang_crc(void);
unsigned long crc(unsigned char *buf, int len);


// ==== LƯU PNG
void luuAnhPNG( char *tenTep, unsigned char *duLieuAnh, unsigned int beRong, unsigned int beCao ) {
   // ---- lọc các hàng ảnh
   unsigned int beDaiDuLieuAnhLoc;
   unsigned char *duLieuAnhLoc = locDuLieuAnh_32bit( duLieuAnh, beRong, beCao, &beDaiDuLieuAnhLoc );

   
   if( duLieuAnhLoc ) {
      // ---- dùng zlib để nén dữ liệu
      int err;
      z_stream c_stream; // cấu trúc cho nén dữ liệu
      
      c_stream.zalloc = (alloc_func)0;
      c_stream.zfree = (free_func)0;
      c_stream.opaque = (voidpf)0;
      
      // ---- xem nếu chuẩn bị có sai lầm nào
      err = deflateInit(&c_stream, kZLIB_MUC_NEN);
      
	   if( err != Z_OK ) {
	      printf( "LuuTapTinPNG: WritePNG: SAI LẦM deflateInit %d (%x) c_stream.avail_in %d\n", err, err, c_stream.avail_out );
      }
      
      // ---- cho dữ liệu cần nén
      c_stream.next_in = duLieuAnhLoc;
      c_stream.avail_in = beDaiDuLieuAnhLoc;
      
      // ---- dự đoán trí nhớ cần cho nén dữ liệu
      unsigned int idat_chunkDataLength = (unsigned int)deflateBound(&c_stream, beDaiDuLieuAnhLoc );
      // 8 bytes for idat length, mã thành phần (tên). Cần mã thành phần cho tính crc
		unsigned char *idat_chunkData = malloc( idat_chunkDataLength + 4);
      
      // ---- đệm cho chứa dữ liệu nén 
      c_stream.next_out  = &(idat_chunkData[4]);
      c_stream.avail_out = idat_chunkDataLength;
      
      err = deflate(&c_stream, Z_FINISH);
      
	   if( err != Z_STREAM_END ) {
	      printf( "LuuTapTinPNG: luuPNG: SAI LẦM deflate %d (%x) c_stream.avail_out %d c_stream.total_out %ld c_stream.avail_in %d\n",
               err, err, c_stream.avail_out, c_stream.total_out, c_stream.avail_in );
      }
      
	   // ---- không cần dữ liệu lọc nữa
	   free( duLieuAnhLoc );
      
      // ==== LƯU
      FILE *dongTapTin = fopen( tenTep, "w" );
      
      if( dongTapTin != NULL ) {
         // ---- ký hiệu tấp tin PNG
         fputc( 0x89, dongTapTin );
         fputc( 0x50, dongTapTin );
         fputc( 0x4e, dongTapTin );
         fputc( 0x47, dongTapTin );
         fputc( 0x0d, dongTapTin );
         fputc( 0x0a, dongTapTin );
         fputc( 0x1a, dongTapTin );
         fputc( 0x0a, dongTapTin );

         // ---- kèm thành phần IHDR
         kemThanhPhanIHDRChoDong( dongTapTin, beRong, beCao, kRGBO, 8 );
         
         // ----
         unsigned int beDaiDuLieuAnhNen = c_stream.total_out;
         unsigned int diaChiDuLieuAnhNen = 0;
         while( diaChiDuLieuAnhNen < beDaiDuLieuAnhNen ) {
            unsigned int beDaiDuLieuThanhPhan = beDaiDuLieuAnhNen - diaChiDuLieuAnhNen;
            if( beDaiDuLieuThanhPhan > kCO_THUOC_TOI_DA_IDAT )
               beDaiDuLieuThanhPhan = kCO_THUOC_TOI_DA_IDAT;

//            printf( "beDaiDuLieuThanhPhan %d   diaChiDuLieuAnhNen %d\n", beDaiDuLieuThanhPhan, diaChiDuLieuAnhNen );
            kemThanhPhanIDATChoDong( dongTapTin, &(idat_chunkData[diaChiDuLieuAnhNen]), beDaiDuLieuThanhPhan );
            diaChiDuLieuAnhNen += kCO_THUOC_TOI_DA_IDAT;
         }


         // ---- kèm thời gian
         kemThanhPhanTIMEChoDong( dongTapTin, 2017, 03, 14, 10, 26, 0);

         // ---- kèm kết thúc
         kemThanhPhanIENDChoDong( dongTapTin);
         
         fclose( dongTapTin );
      }
      else {
         printf( "DocPNG: luuAnhPNG: Vấn đề tạo tệp %s\n", tenTep );
      }
	   // ---- không cần dữ liệu nén nữa
      free( idat_chunkData );
      
      // ---- bỏ c_stream
		deflateEnd( &c_stream );
	}

}

#pragma mark ---- KÈM THÀNH PHẦN
// ======= THÀNH PHẦN ========
void kemThanhPhanIHDRChoDong( FILE *dongTapTin, unsigned int beRong, unsigned int beCao, unsigned char loai, unsigned char bitMoiKenh ) {
   
   unsigned char thanh_phan_ihdr[17];
   
   // ---- bề dài (số lượng byte) của dữ liệu thành phần
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );
   fputc( 13, dongTapTin );
   // ---- mà thành phần
   thanh_phan_ihdr[0] = 'I';
   thanh_phan_ihdr[1] = 'H';
   thanh_phan_ihdr[2] = 'D';
   thanh_phan_ihdr[3] = 'R';
   // ---- bề rộng ảnh
   thanh_phan_ihdr[4] = (beRong & 0xff000000) >> 24;
   thanh_phan_ihdr[5] = (beRong & 0xff0000) >> 16;
   thanh_phan_ihdr[6] = (beRong & 0xff00) >> 8;
   thanh_phan_ihdr[7] = (beRong & 0xff);
   // ---- bề cao ảng
   thanh_phan_ihdr[8] = (beCao & 0xff000000) >> 24;
   thanh_phan_ihdr[9] = (beCao & 0xff0000) >> 16;
   thanh_phan_ihdr[10] = (beCao & 0xff00) >> 8;
   thanh_phan_ihdr[11] = (beCao & 0xff);
   // ---- bit mỗi kênh
   thanh_phan_ihdr[12] = bitMoiKenh;
   // ---- loại ảnh (RGB, RGBO, xám, v.v.)
   thanh_phan_ihdr[13] = loai;
   // ---- phương pháp nén
   thanh_phan_ihdr[14] = 0x00;  // chỉ có một phương pháp
   // ---- phương pháp lọc
   thanh_phan_ihdr[15] = 0x00;  // đổi theo dữ liệu hàng ảnh
   // ---- loại interlace
   thanh_phan_ihdr[16] = 0x00;  // không interlace
   
   // ---- tính mã kiểm tra
   unsigned int maKiemTra = (unsigned int)crc(thanh_phan_ihdr, 17);
   unsigned char chiSo = 0;
   while( chiSo < 17 ) {
      fputc( thanh_phan_ihdr[chiSo], dongTapTin );
      chiSo++;
   }
   
   // ---- lưu mã CRC
   fputc( (maKiemTra & 0xff000000) >> 24, dongTapTin );
   fputc( (maKiemTra & 0xff0000) >> 16, dongTapTin );
   fputc( (maKiemTra & 0xff00) >> 8, dongTapTin );
   fputc( (maKiemTra & 0xff), dongTapTin );
}

void kemThanhPhanTIMEChoDong( FILE *dongTapTin, unsigned short nam, unsigned char thang, unsigned char ngay, unsigned char gio, unsigned char phut, unsigned char giay ) {
   
   unsigned char thanhPhanThoiGian[19];  // chunk length, chunk type, chunk data, CRC
   // ---- bề dài (số lượng byte) của dữ liệu thành phần
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );
   fputc( 7, dongTapTin );
   // ---- chunk type
   thanhPhanThoiGian[0] = 't';
   thanhPhanThoiGian[1] = 'I';
   thanhPhanThoiGian[2] = 'M';
   thanhPhanThoiGian[3] = 'E';
   // ---- năm
   thanhPhanThoiGian[4] = (nam & 0xff00) >> 8;
   thanhPhanThoiGian[5] = nam & 0xff;
   // ---- tháng
   thanhPhanThoiGian[6] = thang;
   // ---- ngày
   thanhPhanThoiGian[7] = ngay;
   // ---- hour
   thanhPhanThoiGian[8] = gio;
   // ---- phút
   thanhPhanThoiGian[9] = phut;
   // ---- giây
   thanhPhanThoiGian[10] = giay;
   // ---- mã kiểm tra
   unsigned int maKiemTra = (unsigned int)crc( thanhPhanThoiGian, 11);
   unsigned char chiSo = 0;
   while( chiSo < 11 ) {
      fputc( thanhPhanThoiGian[chiSo], dongTapTin );
      chiSo++;
   }
   
   // ---- lưu mã CRC
   fputc( (maKiemTra & 0xff000000) >> 24, dongTapTin );
   fputc( (maKiemTra & 0xff0000) >> 16, dongTapTin );
   fputc( (maKiemTra & 0xff00) >> 8, dongTapTin );
   fputc( (maKiemTra & 0xff), dongTapTin );
}


void kemThanhPhanIENDChoDong( FILE *dongTapTin ) {
   
   unsigned char thanhPhan_iend[8];
   // ---- bề dài (số lượng byte) của dữ liệu thành phần (always zero)
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );
   fputc( 0, dongTapTin );

   // ---- mã thàn phần (tên)
   fputc( 'I', dongTapTin );
   fputc( 'E', dongTapTin );
   fputc( 'N', dongTapTin );
   fputc( 'D', dongTapTin );
   
   // ---- mã CRC
   fputc( 0xae, dongTapTin );
   fputc( 0x42, dongTapTin );
   fputc( 0x60, dongTapTin );
   fputc( 0x82, dongTapTin );
}


void kemThanhPhanIDATChoDong( FILE *dongTapTin, unsigned char *duLieuMauAnhNen, unsigned int beDaiDuLieuNen ) {
   
   fputc( (beDaiDuLieuNen & 0xff000000) >> 24, dongTapTin );
   fputc( (beDaiDuLieuNen & 0xff0000) >> 16, dongTapTin );
   fputc( (beDaiDuLieuNen & 0xff00) >> 8, dongTapTin );
   fputc( beDaiDuLieuNen & 0xff, dongTapTin );
   // ---- mã thành phần (tên)
   duLieuMauAnhNen[0] = 'I';
   duLieuMauAnhNen[1] = 'D';
   duLieuMauAnhNen[2] = 'A';
   duLieuMauAnhNen[3] = 'T';
   
   beDaiDuLieuNen += 4;  // <--- cần cộng thêm 4 cho loại thành phần
   unsigned int chiSo = 0;
   while( chiSo < beDaiDuLieuNen ) {
      fputc( duLieuMauAnhNen[chiSo], dongTapTin );
      chiSo++;
   }

   // ---- mã kiểm tra
   unsigned int maKiemTra = (unsigned int)crc( duLieuMauAnhNen, beDaiDuLieuNen );

   // ---- lưu mã CRC
   fputc( (maKiemTra & 0xff000000) >> 24, dongTapTin );
   fputc( (maKiemTra & 0xff0000) >> 16, dongTapTin );
   fputc( (maKiemTra & 0xff00) >> 8, dongTapTin );
   fputc( (maKiemTra & 0xff), dongTapTin );
}


#pragma mark ---- Bộ Lọc PNG
#pragma mark ---- Lọc Dữ Liệu Ảnh
// ---- LƯU Ý: nó lật ngược tậm ảnh
unsigned char *locDuLieuAnh_32bit( unsigned char *duLieuAnh, unsigned short beRong, unsigned short beCao, unsigned int *beDaiDuLieuAnhLoc) {
   
   *beDaiDuLieuAnhLoc = (beRong*beCao << 2) + beCao;  // cần kèm một byte cho mỗi hàng (số bộ lọc cho hàng)
   unsigned char *duLieuAnhLoc = malloc( *beDaiDuLieuAnhLoc );
   
   unsigned short soHang = 0;  // số hàng
	unsigned int diaChiDuLieuLoc = 0;  // địa chỉ trong dữ liệu lọc
	unsigned int diaChiDuLieuAnh = beRong*(beCao - 1) << 2; // bắt đầu tại hàng cuối (lật ngược ảnh)
	unsigned char boLoc;   // số bộ lọc
   
	while( soHang < beCao ) {
      
      // ---- kiểm tra dữ liệu của mỗi hàng và quyết định dùng bộ lọc nào
      //           (không thể xài bộ lọc 2, 3, 4 cho hàng số 0, chỉ được xài loại 0 or 1)
      int tongSoBoLoc0 = 0;   // tổng số của mỗi byte trong hàng (cộng có dấu)
      int tongSoBoLoc1 = 0;   // tổng số sự khác của giữa byte này và 4 byte trước += b[n] - b[n-4]
      int tongSoBoLoc2 = 0;   // tổng số sự khác của giữa byte này và byte hàng trước += b[n] - b[n hàng trước]
      int tongSoBoLoc3 = 0;   //  += b[n] - (b[n hàng trước] + b[n-4])/2
      int tongSoBoLoc4 = 0;   // tổng số paeth
		
		if( soHang != 0 ) {  // chỉ dùng bộ lọc 0 cho hàng 0
         // ---- tổng số bộ lọc 0
         unsigned int soCot = 0;
         while( soCot < (beRong << 2) ) {  // nhân 4 vì có 4 byte cho một điềm ảnh
            tongSoBoLoc0 += (char)duLieuAnh[diaChiDuLieuAnh + soCot];
            tongSoBoLoc0 += (char)duLieuAnh[diaChiDuLieuAnh + soCot + 1];
            tongSoBoLoc0 += (char)duLieuAnh[diaChiDuLieuAnh + soCot + 2];
            tongSoBoLoc0 += (char)duLieuAnh[diaChiDuLieuAnh + soCot + 3];
            soCot += 4;
         }
         // ---- tổng số bộ lọc 1
         tongSoBoLoc1 = (char)duLieuAnh[diaChiDuLieuAnh];
         tongSoBoLoc1 = (char)duLieuAnh[diaChiDuLieuAnh + 1];
         tongSoBoLoc1 = (char)duLieuAnh[diaChiDuLieuAnh + 2];
         tongSoBoLoc1 = (char)duLieuAnh[diaChiDuLieuAnh + 3];
         
		   soCot = 4;
			while( soCot < (beRong << 2) ) {
			   tongSoBoLoc1 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot]
                                   - (int)duLieuAnh[diaChiDuLieuAnh + soCot-4]) & 0xff;
			   tongSoBoLoc1 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot+1]
                                   - (int)duLieuAnh[diaChiDuLieuAnh + soCot-3]) & 0xff;
			   tongSoBoLoc1 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot+2]
                                   - (int)duLieuAnh[diaChiDuLieuAnh + soCot-2]) & 0xff;
			   tongSoBoLoc1 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot+3]
                                   - (int)duLieuAnh[diaChiDuLieuAnh + soCot-1]) & 0xff;
			   soCot += 4;
			};
         // ---- tổng số bộ lọc 2
		   unsigned int diaChiDuLieuAnhHangTruoc = diaChiDuLieuAnh + (beRong << 2);
		   soCot = 0;
			while( soCot < (beRong << 2) ) {
			   tongSoBoLoc2 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot]
                                   - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot]) & 0xff;
			   tongSoBoLoc2 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot+1]
                                   - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot+1]) & 0xff;
			   tongSoBoLoc2 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot+2]
                                   - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot+2]) & 0xff;
			   tongSoBoLoc2 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot+3]
                                   - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot+3]) & 0xff;
			   soCot += 4;
			};
         // ---- tổng số bộ lọc 3
		   diaChiDuLieuAnhHangTruoc = diaChiDuLieuAnh + (beRong << 2);
			// --- điểm ành đầu chỉ xài dữ liệu từ hàng ở trên (đừng xài >> 1 fđể chia 2,  int có dấu)
         tongSoBoLoc3 = (char)((int)duLieuAnh[diaChiDuLieuAnh] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc] / 2)) & 0xff;
         tongSoBoLoc3 = (char)((int)duLieuAnh[diaChiDuLieuAnh + 1] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc+1] / 2)) & 0xff;
         tongSoBoLoc3 = (char)((int)duLieuAnh[diaChiDuLieuAnh + 2] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc+2] / 2)) & 0xff;
         tongSoBoLoc3 = (char)((int)duLieuAnh[diaChiDuLieuAnh + 3] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc+3] / 2)) & 0xff;
         
		   soCot = 4;
			while( soCot < (beRong << 2) ) {
			   tongSoBoLoc3 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot]
                                   - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 4]) / 2) & 0xff;
			   tongSoBoLoc3 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot + 1]
                                   - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot + 1] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 3]) / 2) & 0xff;
			   tongSoBoLoc3 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot + 2]
                                   - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot + 2] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 2]) / 2) & 0xff;
			   tongSoBoLoc3 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot + 3]
                                   - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot + 3] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 1]) / 2) & 0xff;
			   soCot += 4;
			}
         // ---- tổng số bộ lọc 4
		   diaChiDuLieuAnhHangTruoc = diaChiDuLieuAnh + (beRong << 2);
			// --- điểm ảnh đầu chỉ xài dữ liệu từ hàng ở trên
         tongSoBoLoc4 = (char)((int)duLieuAnh[diaChiDuLieuAnh] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc]) & 0xff;
         tongSoBoLoc4 = (char)((int)duLieuAnh[diaChiDuLieuAnh + 1] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc+1]) & 0xff;
         tongSoBoLoc4 = (char)((int)duLieuAnh[diaChiDuLieuAnh + 2] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc+2]) & 0xff;
         tongSoBoLoc4 = (char)((int)duLieuAnh[diaChiDuLieuAnh + 3] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc+3]) & 0xff;
         
		   soCot = 4;
			int a;
			int b;
			int c;
			int duDoan;
			int duDoanA;
			int duDoanB;
			int duDoanC;
         
			while( soCot < (beRong << 2) ) {
			   a = duLieuAnh[diaChiDuLieuAnh + soCot - 4];
			   b = duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot];
			   c = duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot - 4];
            
            duDoan = b - c;
				duDoanC = a - c;
            //				duDoanA = duDoan < 0 ? -duDoan : duDoan;
            //				duDoanB = duDoanC < 0 ? -duDoanC : duDoanC;
            //				duDoanC = (duDoan + duDoanC) < 0 ? -(duDoan + duDoanC) : duDoan + duDoanC;
            
				duDoanA = abs(duDoan);
				duDoanB = abs(duDoanC);
				duDoanC = abs(duDoan + duDoanC);
            
            duDoan = (duDoanA <= duDoanB && duDoanA <= duDoanC) ? a : (duDoanB <= duDoanC) ? b : c;
            
			   tongSoBoLoc4 += (char)((int)duLieuAnh[diaChiDuLieuAnh + soCot] - duDoan) & 0xff;
			   soCot ++;
         }
         
			// ---- giá trị tuyệt đối của việc cộng
			if( tongSoBoLoc0 < 0 )
			   tongSoBoLoc0 = -tongSoBoLoc0;
			if( tongSoBoLoc1 < 0 )
			   tongSoBoLoc1 = -tongSoBoLoc1;
			if( tongSoBoLoc2 < 0 )
			   tongSoBoLoc2 = -tongSoBoLoc2;
			if( tongSoBoLoc3 < 0 )
			   tongSoBoLoc3 = -tongSoBoLoc3;
			if( tongSoBoLoc4 < 0 )
			   tongSoBoLoc4 = -tongSoBoLoc4;
         
         // ---- tìm giá trị bộ lọc nào nhỏ nhất
			boLoc = 0;
			unsigned int boLocNhoNhat = tongSoBoLoc0;
			if( tongSoBoLoc1 < boLocNhoNhat ) {
			   boLoc = 1;
				boLocNhoNhat = tongSoBoLoc1;
         }
         if( tongSoBoLoc2 < boLocNhoNhat ) {
			   boLoc = 2;
				boLocNhoNhat = tongSoBoLoc2;
         }
         if( tongSoBoLoc3 < boLocNhoNhat ) {
			   boLoc = 3;
				boLocNhoNhat = tongSoBoLoc3;
         }
         if( tongSoBoLoc4 < boLocNhoNhat ) {
			   boLoc = 4;
         }
      }
      else {
         boLoc = 0;
		}
      //      NSLog( @"LuuHoaTietPNG: locDuLieuAnh_32bitsố bộ lọc: %d", boLoc );
	   // ---- byte đầu là số bộ lọc (loại bộ lọc)
	   duLieuAnhLoc[diaChiDuLieuLoc] = boLoc;
		// ---- byte tiếp là byte đầu của dữ liệu lọc
		diaChiDuLieuLoc++;
      
      if( boLoc == 0 ) {  // ---- không lọc, chỉ chép dữ liệu
		   unsigned int soCot = 0;
			while( soCot < (beRong << 2) ) {
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot] = duLieuAnh[diaChiDuLieuAnh + soCot];
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot + 1] = duLieuAnh[diaChiDuLieuAnh + soCot + 1];
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot + 2] = duLieuAnh[diaChiDuLieuAnh + soCot + 2];
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot + 3] = duLieuAnh[diaChiDuLieuAnh + soCot + 3];
			   soCot += 4;
			}
      }
		else if( boLoc == 1 ) {  // ---- bộ lọc trừ
			// ---- chép dữ liệu điểm ảnh
         duLieuAnhLoc[diaChiDuLieuLoc] = duLieuAnh[diaChiDuLieuAnh];
         duLieuAnhLoc[diaChiDuLieuLoc + 1] = duLieuAnh[diaChiDuLieuAnh + 1];
         duLieuAnhLoc[diaChiDuLieuLoc + 2] = duLieuAnh[diaChiDuLieuAnh + 2];
         duLieuAnhLoc[diaChiDuLieuLoc + 3] = duLieuAnh[diaChiDuLieuAnh + 3];
         
		   unsigned int soCot = 4;
			while( soCot < (beRong << 2) ) {
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot]
                                                     - (int)duLieuAnh[diaChiDuLieuAnh + soCot-4]) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot+1] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot+1]
                                                       - (int)duLieuAnh[diaChiDuLieuAnh + soCot-3]) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot+2] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot+2]
                                                       - (int)duLieuAnh[diaChiDuLieuAnh + soCot-2]) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot+3] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot+3]
                                                       - (int)duLieuAnh[diaChiDuLieuAnh + soCot-1]) & 0xff;
			   soCot += 4;
			};
		}
		else if( boLoc == 2 ) {  // ---- bộ lọc lên
		   unsigned int diaChiDuLieuAnhHangTruoc = diaChiDuLieuAnh + (beRong << 2);
		   unsigned int soCot = 0;
			while( soCot < (beRong << 2) ) {
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot]
                                                     - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot]) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot+1] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot+1]
                                                       - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot+1]) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot+2] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot+2]
                                                       - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot+2]) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot+3] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot+3]
                                                       - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot+3]) & 0xff;
			   soCot += 4;
			};
		}
		else if( boLoc == 3 ) {  // ---- bộ lọc trung bình
		   unsigned int diaChiDuLieuAnhHangTruoc = diaChiDuLieuAnh + (beRong << 2);
			// --- điểm ành đầu chỉ xài dữ liệu từ hàng ở trên
         // LƯU Ý: đừng dùng >> 1 để chia 2, int có dấu)
         duLieuAnhLoc[diaChiDuLieuLoc] = ((int)duLieuAnh[diaChiDuLieuAnh] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc] / 2)) & 0xff;
         duLieuAnhLoc[diaChiDuLieuLoc+1] = ((int)duLieuAnh[diaChiDuLieuAnh + 1] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc+1] / 2)) & 0xff;
         duLieuAnhLoc[diaChiDuLieuLoc+2] = ((int)duLieuAnh[diaChiDuLieuAnh + 2] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc+2] / 2)) & 0xff;
         duLieuAnhLoc[diaChiDuLieuLoc+3] = ((int)duLieuAnh[diaChiDuLieuAnh + 3] - (int)(duLieuAnh[diaChiDuLieuAnhHangTruoc+3] / 2)) & 0xff;
         
		   unsigned int soCot = 4;
			while( soCot < (beRong << 2) ) {
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot]
                                                     - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 4]) / 2) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot + 1] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot + 1]
                                                         - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot + 1] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 3]) / 2) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot + 2] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot + 2]
                                                         - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot + 2] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 2]) / 2) & 0xff;
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot + 3] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot + 3]
                                                         - ((int)duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot + 3] + (int)duLieuAnh[diaChiDuLieuAnh + soCot - 1]) / 2) & 0xff;
			   soCot += 4;
			}
      }
		else if( boLoc == 4 ) {  // ---- bộ lọc paeth
		   unsigned int diaChiDuLieuAnhHangTruoc = diaChiDuLieuAnh + (beRong << 2);
			// --- điểm ảnh đầu tiên của hàng chỉ xài dữ liệu từ điểm ảnh ở hàng trên
         duLieuAnhLoc[diaChiDuLieuLoc] = ((int)duLieuAnh[diaChiDuLieuAnh] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc]) & 0xff;
         duLieuAnhLoc[diaChiDuLieuLoc+1] = ((int)duLieuAnh[diaChiDuLieuAnh + 1] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc+1]) & 0xff;
         duLieuAnhLoc[diaChiDuLieuLoc+2] = ((int)duLieuAnh[diaChiDuLieuAnh + 2] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc+2]) & 0xff;
         duLieuAnhLoc[diaChiDuLieuLoc+3] = ((int)duLieuAnh[diaChiDuLieuAnh + 3] - (int)duLieuAnh[diaChiDuLieuAnhHangTruoc+3]) & 0xff;
         
		   unsigned int soCot = 4;
			int a;
			int b;
			int c;
			int duDoan;   // dự đoán
			int duDoanA;  // dự đoán A
			int duDoanB;  // dự đoán B
			int duDoanC;  // dự đoán C
         
			while( soCot < (beRong << 2) ) {
			   a = duLieuAnh[diaChiDuLieuAnh + soCot - 4];
			   b = duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot];
			   c = duLieuAnh[diaChiDuLieuAnhHangTruoc + soCot - 4];
            
            duDoan = b - c;
				duDoanC = a - c;
            //				duDoanA = duDoan < 0 ? -duDoan : duDoan;
            //				duDoanB = duDoanC < 0 ? -duDoanC : duDoanC;
            //				duDoanC = (duDoan + duDoanC) < 0 ? -(duDoan + duDoanC) : duDoan + duDoanC;
            
				duDoanA = abs(duDoan);
				duDoanB = abs(duDoanC);
				duDoanC = abs(duDoan + duDoanC);
            
            duDoan = (duDoanA <= duDoanB && duDoanA <= duDoanC) ? a : (duDoanB <= duDoanC) ? b : c;
            
			   duLieuAnhLoc[diaChiDuLieuLoc + soCot] = ((int)duLieuAnh[diaChiDuLieuAnh + soCot] - duDoan) & 0xff;
			   soCot++;
         }
		}
		else {   // ---- loại lọc không biết
		   ; 
		}
      // ---- chuần bị cho hàng tiếp
		soHang++;
		diaChiDuLieuLoc += (beRong << 2);
		diaChiDuLieuAnh -= (beRong << 2);
   }
   
   return duLieuAnhLoc;
}


#pragma mark ---- CRC

unsigned long crc_table[256];

// cờ: đã tính bảng CRC chưa? Đầu tiên chưa tính.
int bang_crc_da_tinh = 0;

// bảng cho tính mã CRC lẹ.
void tao_bang_crc(void) {
   unsigned long c;
   int n, k;
   
   for (n = 0; n < 256; n++) {
      c = (unsigned long) n;
      for (k = 0; k < 8; k++) {
         if (c & 1)
            c = 0xedb88320L ^ (c >> 1);  // 1110 1101 1011 1000 1000 0011 0010 0000
         else
            c = c >> 1;
      }
      crc_table[n] = c;
   }
   // ---- đặt đã tính rồi
   bang_crc_da_tinh = 1;
}

// Nâng cấp CRC đang chảy với byte trong buf[0..len-1]
// -- khi khởi động nên đặt toàn bộ mã CRC bằng bit 1, and the transmitted value
//	  is the 1s complement of the final running CRC (xem hàm crc() ở dưới)).

unsigned long nang_cap_crc(unsigned long crc, unsigned char *buf, int len) {
   
   unsigned long c = crc;
   int n;
   
   if (!bang_crc_da_tinh)
      tao_bang_crc();
   
   for (n = 0; n < len; n++) {
      c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
   }
   return c;
}

// Return the CRC of the bytes buf[0..len-1].
unsigned long crc(unsigned char *buf, int len) {
   return nang_cap_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}


// ==== ĐỌC PNG
unsigned char *docTapTinPNG( FILE *dongTapTin, unsigned int *beRong, unsigned int *beCao, unsigned char *bitChoDiemAnh, unsigned long *doDaiDuLieuDaDoc );
void docDauTapTinTuDuLieu( FILE *dongDuLieu, unsigned int *beRong, unsigned int *beCao, unsigned char *bitChoDiemAnh, unsigned char *thuMau, unsigned char *nen, unsigned char *boLoc, unsigned char *loaiInterlace );
unsigned char *locNguocDuLieuDiemAnh_32Bit( unsigned char *duLieuDaLoc,  unsigned short beRong, unsigned short beCao );
void nhanDucCuaAnh( unsigned char *duLieuAnh, unsigned int beRong, unsigned int beCao );


#pragma mark ---- Đọc tập tin PNG BGRO (32 bit)
unsigned char *docPNG_BGRO(char *duongTapTin, unsigned int *beRong, unsigned int *beCao, unsigned char *canLatMau) {
   
	if( duongTapTin ) {
      
   	unsigned char bitChoDiemAnh;    // số lượng bit cho một điểm ảnh
	   unsigned long int compressedTextureDataLength;
      
      FILE *dongTapTin = fopen( duongTapTin, "r" );
      
      if( dongTapTin != NULL ) {

         unsigned char *duLieuAnhNen = docTapTinPNG( dongTapTin, beRong, beCao, &bitChoDiemAnh, &compressedTextureDataLength);
         
         // ---- độ dài cho dữ liệu sau rã (vẫn còn cần lọc ngược mỗi hàng)
         unsigned long int doDaiDuLieuSauRa = ((*beRong)*(*beCao) << 2) + *beCao;
         unsigned char *duLieuRa = malloc( doDaiDuLieuSauRa );  // đệm cho dữ liệu rã
         
         // ---- xài zlib ch rã dữ liệu ảnh
         int xaiLamZLIB;
         z_stream d_stream; // cấu trúc nén dòng dữ liệu
         
         d_stream.zalloc = (alloc_func)0;
         d_stream.zfree = (free_func)0;
         d_stream.opaque = (voidpf)0;
         d_stream.data_type = Z_BINARY;
         
         d_stream.next_in  = duLieuAnhNen;
         d_stream.avail_in = (unsigned int)compressedTextureDataLength;
         
         // ---- xem sau chuẩn bị có sai lầm gì
         if( (duLieuAnhNen[0] & 0x08) == 0x08 ) {   // chỉ kiểm trà 4 bit thấp = 0x8
            xaiLamZLIB = inflateInit(&d_stream);
            *canLatMau = kSAI;
         }
         else {
            xaiLamZLIB = inflateInit2(&d_stream, -15);
            *canLatMau = kDUNG;
         }
         
         if( xaiLamZLIB != Z_OK ) {
            printf( "DocHoaTietPNG: docTapTinPNG: sai lầm inflateInit %d (%x) d_stream.avail_in %d\n", xaiLamZLIB, xaiLamZLIB, d_stream.avail_in );
         }
         
         // ---- cho dữ liệu cần rã
         d_stream.next_out = duLieuRa;
         d_stream.avail_out = (uInt)( doDaiDuLieuSauRa );
         
         xaiLamZLIB = inflate(&d_stream, Z_STREAM_END);
         
         if( xaiLamZLIB != Z_STREAM_END ) {
            printf( "DocHoaTietPNG: docTapTinPNG: sai lầm inflate %d (%x) d_stream.avail_out %d d_stream.total_out %lu\n",
                   xaiLamZLIB, xaiLamZLIB, d_stream.avail_out, d_stream.total_out );
            
         }
         
         // ---- không cần dữ liệu nén nữa
         free( duLieuAnhNen );
         
         // ---- lọc ngược
         // dữ liệu PNG là 4 byte R G B O sát bên nhau
         unsigned char *duLieuAnhKhongLoc = locNguocDuLieuDiemAnh_32Bit( duLieuRa, *beRong, *beCao);
         
         return duLieuAnhKhongLoc;
      }
      else {
         printf( "DocPNG_BGRO: Không thể mở tệp %s\n", duongTapTin );
         exit(0);
      }
	}
   return NULL;
}

#pragma mark ---- Đọc Đầu Tập Tin
void docDauTapTinTuDuLieu( FILE *dongDuLieu, unsigned int *beRong, unsigned int *beCao, unsigned char *bitChoDiemAnh, unsigned char *thuMau, unsigned char *nen, unsigned char *boLoc, unsigned char *loaiInterlace ) {

   *beRong = fgetc( dongDuLieu ) << 24 | fgetc( dongDuLieu ) << 16 | fgetc( dongDuLieu ) << 8 | 
			                        fgetc( dongDuLieu );

   *beCao = fgetc( dongDuLieu ) << 24 | fgetc( dongDuLieu ) << 16 | fgetc( dongDuLieu ) << 8 | 
			                        fgetc( dongDuLieu );

// printf( "DocTapTinPNG: docTapTinPNG: khổ ảnh %d (%x)  %d (%x)\n", *beRong, *beRong, *beCao, *beCao );
   // ---- bit cho một điểm ảnh
	*bitChoDiemAnh = fgetc( dongDuLieu );

   // ---- thứ dữ liệu màu (xám, bảng màu, RGBO, v.v.)
	*thuMau = fgetc( dongDuLieu );

   // ---- nén
	*nen = fgetc( dongDuLieu );
//	printf( "Thứ nén: %d\n", *nen );

   // ---- bộ lọc
	*boLoc = fgetc( dongDuLieu );

        *loaiInterlace = fgetc( dongDuLieu );
}


unsigned char *docTapTinPNG( FILE *dongTapTin, unsigned int *beRong, unsigned int *beCao, unsigned char *bitChoDiemAnh, unsigned long *doDaiDuLieuDaDoc) {
   
   *doDaiDuLieuDaDoc = 0;  // chưa đọc gì cả, đặt = 0
   unsigned char *duLieuNen = NULL;    // dữ liệu bị nén từ tập tin PNG
   unsigned char thuMau;   // thứ dữ liệu màu trong tập tin
	unsigned char nen;      // phương pháp nén (hình như chuẩn PNG chỉ có một phương pháp)
	unsigned char boLoc;    // bộ lọc trước nén dữ liệu
	unsigned char loaiInterlace;
   unsigned int doDaiDuLieiIDAT = 0;  // độ dài dữ liệu của tất cả thành phần IDAT
	unsigned int soLuongThanhPhanIDAT = 0;  // cho đếm số lượng thành phần IDAT
   
   // mảng cko thành phần IDAT; nên không có hơn 2048 cái ^-^ 8192 * 2048 = 16 777 216 byte
   unsigned long int IDAT_viTriThanhPhan[2048];
   unsigned long int IDAT_doDaiThanhPhan[2048];
   
   
   if( dongTapTin != NULL ) {
      
      //      NSLog( @"DocHoaTietPNG: docPNG: duongTaptin %@", duongTapTin );
      
      // ---- đọc ký hiệu tập tin, cho biết tập tin này là dạng ảnh PNG
      unsigned long kyHieuTapTin;
      
      unsigned int phanDau = fgetc( dongTapTin ) << 24 | fgetc( dongTapTin ) << 16 | fgetc( dongTapTin ) << 8 | fgetc( dongTapTin );
      unsigned int phanCuoi = fgetc( dongTapTin ) << 24 | fgetc( dongTapTin ) << 16 | fgetc( dongTapTin ) << 8 | fgetc( dongTapTin );
      kyHieuTapTin = (unsigned long)phanDau << 32 | phanCuoi;
      //      printf( "%lx   %x   %x\n", kyHieuTapTin, phanDau, phanCuoi );
      
      // ---- xem ký hiệu tập tin
      if( kyHieuTapTin ==  0x89504e470d0a1a0a ) {
         
		   unsigned int doDaiThanhPhan;
		   unsigned int loaiThanhPhan;
		   unsigned int maCRC;  // mã số CRC
         
		   // ---- đọc các thành phần
         
		   unsigned char docHetThanhPhanChua = kSAI;
         
         while( !docHetThanhPhanChua ) {
		      // ---- đọc độ dài thành phần
	   	   doDaiThanhPhan = fgetc( dongTapTin ) << 24 | fgetc( dongTapTin ) << 16 | fgetc( dongTapTin ) << 8 | fgetc( dongTapTin );
            
		      // ---- đọc thứ thành phần
            loaiThanhPhan = fgetc( dongTapTin ) << 24 | fgetc( dongTapTin ) << 16 | fgetc( dongTapTin ) << 8 | fgetc( dongTapTin );
            
            //           printf( "DocTapTinPNG: docTapTinPNG: doDaiThanhPhan %u (%x)   loaiThanhPhan %c%c%c%c\n", doDaiThanhPhan, doDaiThanhPhan, loaiThanhPhan >> 24, loaiThanhPhan >> 16, loaiThanhPhan >> 8, loaiThanhPhan );
            
            if( loaiThanhPhan == kTHANH_PHAN__LOAI_IHDR ) {
               docDauTapTinTuDuLieu( dongTapTin, beRong, beCao, bitChoDiemAnh, &thuMau, &nen, &boLoc, &loaiInterlace);
//               printf( "kho %d %d  bit %d thuMau %d\n", *beRong, *beCao, *bitChoDiemAnh, thuMau );
               // ---- nếu ảnh quá lớn không đọc và trở lại sớm
					if( (*beRong > kKHO_ANH_TOI_DA) || (*beCao > kKHO_ANH_TOI_DA) ) {
					   return NULL;
					}
               
            }
            /*			   else if( loaiThanhPhan == kTHANH_PHAN__LOAI_pHYs ) {
             unsigned int beRongDiemAnh;
             unsigned int beCaoDiemAnh;
             
             tamDuLieu.length = 4;
             [dongTapTin getBytes:&beRongDiemAnh range:tamDuLieu];
             tamDuLieu.location += 4;
             beRongDiemAnh = (beRongDiemAnh & 0xff) << 24 | (beRongDiemAnh & 0xff00) << 8 | (beRongDiemAnh & 0xff0000) >> 8 |
             (beRongDiemAnh & 0xff000000) >> 24;
             
             [dongTapTin getBytes:&beCaoDiemAnh range:tamDuLieu];
             tamDuLieu.location += 4;
             beCaoDiemAnh = (beCaoDiemAnh & 0xff) << 24 | (beCaoDiemAnh & 0xff00) << 8 | (beCaoDiemAnh & 0xff0000) >> 8 |
             (beCaoDiemAnh & 0xff000000) >> 24;
             // ---- đơn vị
             unsigned char donVi;
             tamDuLieu.length = 1;
             [dongTapTin getBytes:&donVi range:tamDuLieu];
             tamDuLieu.location += 1;
             if( donVi )
             NSLog( @"DocTapTinPNG: docTapTinPNG: cỡ thước điểm ảnh %d  %d pixels/mét (%d)\n",  beRongDiemAnh, beCaoDiemAnh, donVi );
             else
             NSLog( @"DocTapTinPNG: docTapTinPNG: cỡ thước điểm ảnh %d  %d không biết đơn vị (%d)\n",  beRongDiemAnh, beCaoDiemAnh, donVi );
             
             } */
   	   	else if( loaiThanhPhan == kTHANH_PHAN__LOAI_IDAT ) {
               
               IDAT_viTriThanhPhan[soLuongThanhPhanIDAT] = ftell( dongTapTin );
               IDAT_doDaiThanhPhan[soLuongThanhPhanIDAT] = doDaiThanhPhan;
               //               printf( "IDAT chunkNumber %d  chunkOffset %ld  doDaiThanhPhan %ld\n", soLuongThanhPhanIDAT, IDAT_viTriThanhPhan[soLuongThanhPhanIDAT],
               //                                                IDAT_doDaiThanhPhan[soLuongThanhPhanIDAT] );
               // ---- nhảy qua thành phần này
               fseek( dongTapTin, doDaiThanhPhan, SEEK_CUR );
					// ---- đọc độ dài
					doDaiDuLieiIDAT += doDaiThanhPhan;
				   soLuongThanhPhanIDAT++;
            }
            /*			else if( loaiThanhPhan == kTHANH_PHAN__LOAI_sBIT ) {   // số lượng bit cho màu đỏ, lục, xanh
             
             if( doDaiThanhPhan == 4 ) {
             unsigned int soLuongBitDo = fgetc( filePtr );
             unsigned int soLuongBitLuc = fgetc( filePtr );
             unsigned int soLuongBitXanh = fgetc( filePtr );
             unsigned int soLuongBitDuc = fgetc( filePtr );
             printf( "số lượng bit đỏ %d  lục %d  xanh %d  đục %d\n", soLuongBitDo, soLuongLuc, soLuongXanh, soLuongDuc );
             }
             else if( doDaiThanhPhan == 3 ) {
             unsigned int soLuongBitDo = fgetc( filePtr );
             unsigned int soLuongBitLuc = fgetc( filePtr );
             unsigned int soLuongBitXanh = fgetc( filePtr );
             
             printf( "số lượng bit đỏ %d  lục %d  xanh %d\n", soLuongBitDo, soLuongBitLuc, soLuongBitXanh );
             }
             
             else if( doDaiThanhPhan == 2 ) {
             unsigned int soLuongBitXam = fgetc( filePtr );   // số lượng bit cho kênh xạm
             unsigned int soLuongBitDuc = fgetc( filePtr );   // số lượng bit cho kênh đục
             printf( "số lượng bit xám %d  đục %d\n", soLuongBitXam, soLuongBitDuc );
             }
             else if( doDaiThanhPhan == 1 ) {
             unsigned int soLuongBitXam = fgetc( filePtr );
             printf( "số lượng bit xám %d\n", soLuongBitXam );
             }
             }
             else if( loaiThanhPhan == kTHANH_PHAN__LOAI_bKGD ) {  // màu cho cảnh sau
             if( doDaiThanhPhan == 6 ) {
             unsigned short mauDoCanhSau = fgetc( filePtr ) << 8 | fgetc( filePtr );
             unsigned short mauLucCanhSau = fgetc( filePtr ) << 8 | fgetc( filePtr );
             unsigned short mauXanhCanhSau = fgetc( filePtr ) << 8 | fgetc( filePtr );
             printf( "màu cho cảnh sau (ĐLX) %d %d %d\n", mauDoCanhSau, mauLucCanhSau, mauXanhCanhSau );
             }
             else if( doDaiThanhPhan == 2 ) {
             unsigned short mauXamCanhSau = fgetc( filePtr ) << 8 | fgetc( filePtr );
             printf( "cảnh màu xám %d\n", mauXamCanhSau );
             }
             }
             
             else if( loaiThanhPhan == kTHANH_PHAN__LOAI_tIME ) {
             unsigned short nam = fgetc( filePtr ) << 8 | fgetc( filePtr );
             unsigned char thang = fgetc( filePtr );
             unsigned char ngay = fgetc( filePtr );
             unsigned char gio = fgetc( filePtr );
             unsigned char phut = fgetc( filePtr );
             unsigned char giay = fgetc( filePtr );
             printf( "Năm %d  tháng %d  ngày %d   giờ %d phút %d  giây %d\n", nam, thang, ngay, gio, phut, giay );
             }
             else if( loaiThanhPhan == kTHANH_PHAN__LOAI_sRGB ) {  // ý định chiếu ảnh
             unsigned char renderingIntent;
             tamDuLieu.length = 1;
             [dongTapTin getBytes:&renderingIntent range:tamDuLieu];
             tamDuLieu.location += 1;
             
             if( renderingIntent == 0 )
             printf("Rendering intent perceptual (%d)\n", renderingIntent );
             else if( renderingIntent == 1 )
             printf("Rendering intent relative chromatic (%d)\n", renderingIntent );
             else if( renderingIntent == 2 )
             printf("Rendering intent preserve saturation (%d)\n", renderingIntent );
             else if( renderingIntent == 3 )
             printf("Rendering intent absolute chromatic (%d)\n", renderingIntent );
             else
             printf("Rendering intent unknown (%d)", renderingIntent );
             } */
            else {  // ---- không biết thứ thành phần, bỏ nó
               fseek( dongTapTin, doDaiThanhPhan, SEEK_CUR );
			   }
            
            
            // ---- xem nếu đã tới kết thúc tập tin
            if( loaiThanhPhan == kTHANH_PHAN__LOAI_IEND )
               docHetThanhPhanChua = kDUNG;
            
            // ---- đọc CRC của thành phần
            else {
               
               maCRC = fgetc( dongTapTin) << 24 | fgetc( dongTapTin) << 16 | fgetc( dongTapTin) << 8 | fgetc( dongTapTin);
               
               // ---- đảo ngược cho Intel
               maCRC = (maCRC & 0xff) << 24 | (maCRC & 0xff00) << 8 | (maCRC & 0xff0000) >> 8 | (maCRC & 0xff000000) >> 24;
               
            }
            
         }  // lặp while khi chưa đọc hết thành phần trong tập tin
         
         //      NSLog( @"DocTapTinPNG: docTapTinPNG: Total IDAT chunks number %d  length %d\n", soLuongThanhPhanIDAT, doDaiThanhPhan );
         
         // ---- bấy giờ biết số lượng thành phân IDAT và nó ở đâu, đọc và gồm lại hết cho rã nén
         // byte đầu tiên của IDAT thứ nhất nên như 0x78 hay 0x58, zlib
         // ---- đọc các thanh phần IDAT
         if( doDaiDuLieiIDAT > 0 ) {  // có lẻ không có hay khônh có dữ liệu
            unsigned int chiSoThanhPhanIDAT = 0;  // chỉ số thành phần IDAT
            //            unsigned int *doDaiDuLieuDaDoc = 0;  // độ dài dữ liệu IDAT đã đọc
            duLieuNen = malloc( doDaiDuLieiIDAT );
            unsigned char *dauDemDuLieuNen = duLieuNen;
            
            while( chiSoThanhPhanIDAT < soLuongThanhPhanIDAT ) {
               // ---- tầm cho thân` phân IDAT này
               
               unsigned int beDaiDuLieu = IDAT_doDaiThanhPhan[chiSoThanhPhanIDAT];
               fseek( dongTapTin, IDAT_viTriThanhPhan[chiSoThanhPhanIDAT], SEEK_SET );
      //         printf( "số thành phần %d  dịch %ld  đồ dài %ld\n", chiSoThanhPhanIDAT, IDAT_viTriThanhPhan[chiSoThanhPhanIDAT],
      //                               IDAT_doDaiThanhPhan[chiSoThanhPhanIDAT] );
               
               // ---- đọc dữ liệu cho thành phân IDAT
               unsigned int chiSo = 0;
               while( chiSo < beDaiDuLieu ) {
                  *dauDemDuLieuNen = fgetc( dongTapTin );
                  dauDemDuLieuNen++;
                  chiSo++;
               }
               
               // ---- cộng độ dài thêm
               *doDaiDuLieuDaDoc += beDaiDuLieu;
               // ---- thành phần tiếp
               chiSoThanhPhanIDAT++;
            }
         }
         // ---- sẽ rã nén ở ngòai đây, cần biết độ dài của dữ liệu nén
         //		 *imageDataBufferLength = doDaiThanhPhan;
         
	   }
	   else {
		   printf( "DocTapTinPNG: tập tin không phải PNG, ký hiệu %lx\n", kyHieuTapTin );
			return NULL;
	   }
   }
   
	return duLieuNen;
}


#pragma mark ---- Lọc Ngược Dữ Liệu
// --- CẢNH CÁO: Nó lật ngược tấm ảnh
unsigned char *locNguocDuLieuDiemAnh_32Bit(unsigned char *duLieuDaLoc,  unsigned short beRong, unsigned short beCao) {

   unsigned char *duLieuAnhKhongLoc = malloc( beRong*beCao << 2 );  // nhân 4 vì ảnh có điểm ảnh 32 bit

   unsigned short soHang = 0;   // số hàng đang rã
	unsigned int diaChiTrongDuLieuNen = 0;  // địa chỉ trong dữ liệu nén
	unsigned int diaChiAnh = beRong*(beCao - 1) << 2;  // địa chỉ trong dữ liệu ảnh
	unsigned char boLoc;

	while( soHang < beCao ) {
	   // ---- byte đầu tiên của mỗi hàng là thứ bộ lọc nên áp dụng
	   boLoc = duLieuDaLoc [diaChiTrongDuLieuNen];
		// ---- byte tiếp là bắt đầu dữ liệu bị lọc và nén
		diaChiTrongDuLieuNen++;

		if( boLoc == 0 ) { // ---- không lọc, chỉ cần chép hàng này
		   unsigned int soCot = 0;  // số cột
		   unsigned int soCotCuoi = beRong << 2;  // số cột cuối nhân 4 (4 byte một điểm ảnh)
			while( soCot < soCotCuoi ) {
			   duLieuAnhKhongLoc[diaChiAnh + soCot] = duLieuDaLoc [diaChiTrongDuLieuNen + soCot];
			   duLieuAnhKhongLoc[diaChiAnh + soCot + 1] = duLieuDaLoc [diaChiTrongDuLieuNen + soCot + 1];
			   duLieuAnhKhongLoc[diaChiAnh + soCot + 2] = duLieuDaLoc [diaChiTrongDuLieuNen + soCot + 2];
			   duLieuAnhKhongLoc[diaChiAnh + soCot + 3] = duLieuDaLoc [diaChiTrongDuLieuNen + soCot + 3];
			   soCot += 4;
			}
		}
		else if( boLoc == 1 ) { // ---- bộ lọc trừ giá trị bên trái

			// ---- chép điểm ảnh đầu
         duLieuAnhKhongLoc[diaChiAnh] = duLieuDaLoc [diaChiTrongDuLieuNen];
         duLieuAnhKhongLoc[diaChiAnh + 1] = duLieuDaLoc [diaChiTrongDuLieuNen + 1];
         duLieuAnhKhongLoc[diaChiAnh + 2] = duLieuDaLoc [diaChiTrongDuLieuNen + 2];
         duLieuAnhKhongLoc[diaChiAnh + 3] = duLieuDaLoc [diaChiTrongDuLieuNen + 3];

		   unsigned int soCot = 4;
		   unsigned int soCotCuoi = beRong << 2;  // số cột cuối nhân 4 (4 byte một điểm ảnh)
			while( soCot < soCotCuoi ) {
			   duLieuAnhKhongLoc[diaChiAnh + soCot] = ((int)duLieuAnhKhongLoc[diaChiAnh + soCot-4]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot]) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot+1] = ((int)duLieuAnhKhongLoc[diaChiAnh + soCot-3]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot+1]) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot+2] = ((int)duLieuAnhKhongLoc[diaChiAnh + soCot-2]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot+2]) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot+3] = ((int)duLieuAnhKhongLoc[diaChiAnh + soCot-1]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot+3]) & 0xff;
			   soCot += 4;
			};
		}
		else if( boLoc == 2 ) { // ---- bộ lọc trừ giạ trị từ trên (KHÔNG BAO GIỜ XÀI CHO HÀNG SỐ 0)
		   unsigned int diaChiHangTruocKhongLoc = diaChiAnh + (beRong << 2);   // dịa chỉ hàng trước không lọc
		   unsigned int soCot = 0;
		   unsigned int soCotCuoi = beRong << 2;  // số cột cuối nhân 4 (4 byte một điểm ảnh)
			while( soCot < soCotCuoi ) {
			   duLieuAnhKhongLoc[diaChiAnh + soCot] = ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot]) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot+1] = ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot+1]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot+1]) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot+2] = ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot+2]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot+2]) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot+3] = ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot+3]
				        + (int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot+3]) & 0xff;
			   soCot += 4;
			};
		}
		else if( boLoc == 3 ) { // ---- bộ lọc trung bình (KHÔNG BAO GIỜ XÀI CHO HÀNG SỐ 0)
		   unsigned int diaChiHangTruocKhongLoc = diaChiAnh + (beRong << 2);  // dịa chỉ hàng trước không lọc
			// --- first pixel uses only pixel from above row (don't use >> 1 for dividing by 2, signed int value)
         duLieuAnhKhongLoc[diaChiAnh] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen] + (int)(duLieuAnhKhongLoc[diaChiHangTruocKhongLoc] / 2)) & 0xff;
         duLieuAnhKhongLoc[diaChiAnh + 1] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen+1] + (int)(duLieuAnhKhongLoc[diaChiHangTruocKhongLoc+1] / 2)) & 0xff;
         duLieuAnhKhongLoc[diaChiAnh + 2] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen+2] + (int)(duLieuAnhKhongLoc[diaChiHangTruocKhongLoc+2] / 2)) & 0xff;
         duLieuAnhKhongLoc[diaChiAnh + 3] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen+3] + (int)(duLieuAnhKhongLoc[diaChiHangTruocKhongLoc+3] / 2)) & 0xff;

		   unsigned int soCot = 4;
		   unsigned int soCotCuoi = beRong << 2;  // số cột cuối nhân 4 (4 byte một điểm ảnh)
			while( soCot < soCotCuoi ) {
			   duLieuAnhKhongLoc[diaChiAnh + soCot] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot] +
				                  ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot] + (int)duLieuAnhKhongLoc[diaChiAnh + soCot - 4]) / 2) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot + 1] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot + 1] +
				                  ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot + 1] + (int)duLieuAnhKhongLoc[diaChiAnh + soCot - 3]) / 2) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot + 2] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot + 2] +
				                  ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot + 2] + (int)duLieuAnhKhongLoc[diaChiAnh + soCot - 2]) / 2) & 0xff;
			   duLieuAnhKhongLoc[diaChiAnh + soCot + 3] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot + 3] +
				                  ((int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot + 3] + (int)duLieuAnhKhongLoc[diaChiAnh + soCot - 1]) / 2) & 0xff;
			   soCot += 4;
			}
		}
		else if( boLoc == 4 ) { // ---- bộ lọc paeth (KHÔNG BAO GIỜ XÀI CHO HÀNG SỐ 0)
		   unsigned int diaChiHangTruocKhongLoc = diaChiAnh + (beRong << 2);  // dịa chỉ hàng trước không lọc
			// --- first pixel uses only pixel from above row
         duLieuAnhKhongLoc[diaChiAnh] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen] + (int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc]) & 0xff;
         duLieuAnhKhongLoc[diaChiAnh + 1] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen+1] + (int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc+1]) & 0xff;
         duLieuAnhKhongLoc[diaChiAnh + 2] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen+2] + (int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc+2]) & 0xff;
         duLieuAnhKhongLoc[diaChiAnh + 3] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen+3] + (int)duLieuAnhKhongLoc[diaChiHangTruocKhongLoc+3]) & 0xff;

		   unsigned int soCot = 4;
			int a;
			int b;
			int c;
			int duDoan;
			int duDoanA;
			int duDoanB;
			int duDoanC;

			while( soCot < (beRong << 2) ) {
			   a = duLieuAnhKhongLoc[diaChiAnh + soCot - 4];
			   b = duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot];
			   c = duLieuAnhKhongLoc[diaChiHangTruocKhongLoc + soCot - 4];

            duDoan = b - c;
				duDoanC = a - c;

				duDoanA = duDoan < 0 ? -duDoan : duDoan;
				duDoanB = duDoanC < 0 ? -duDoanC : duDoanC;
				duDoanC = (duDoan + duDoanC) < 0 ? -(duDoan + duDoanC) : duDoan + duDoanC;

//				duDoanA = abs(duDoan);
//				duDoanB = abs(duDoanC);
//				duDoanC = abs(duDoan + duDoanC);

            duDoan = (duDoanA <= duDoanB && duDoanA <= duDoanC) ? a : (duDoanB <= duDoanC) ? b : c;

			   duLieuAnhKhongLoc[diaChiAnh + soCot] = ((int)duLieuDaLoc [diaChiTrongDuLieuNen + soCot] + duDoan) & 0xff;
			   soCot++;
         }
		}
		else {
		   printf( "DocHoaTietPNG: locNguoc: gặp hàng có bộ lọc lạ %d\n", boLoc );
		}

      // ---- prepare for next row
		soHang++;
		diaChiTrongDuLieuNen += (beRong << 2);
		diaChiAnh -= (beRong << 2);
	}

	return duLieuAnhKhongLoc;
}


#pragma mark ---- Nhân Đục
void nhanDucCuaAnh( unsigned char *duLieuAnh, unsigned int beRong, unsigned int beCao) {

   unsigned int diaChiAnh = 0;
   unsigned short hang = 0;
   while( hang < beCao ) {
      unsigned short cot = 0;
      while( cot < beRong ) {
         diaChiAnh = (hang*beRong + cot) << 2;

         unsigned int duc = duLieuAnh[diaChiAnh+3];
         duLieuAnh[diaChiAnh] = (duc*(int)duLieuAnh[diaChiAnh]) >> 8;
         duLieuAnh[diaChiAnh+1] = (duc*(int)duLieuAnh[diaChiAnh+1]) >> 8;
         duLieuAnh[diaChiAnh+2] = (duc*(int)duLieuAnh[diaChiAnh+2]) >> 8;
         cot++;
      }
      hang++;
   }
}


