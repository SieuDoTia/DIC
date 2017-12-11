//  meoHoa.c   // méoHóa.c
//  Phiên bản 1.0
//  Phát hành 2560/12/11
//  Cho ảnh PNG
//  Khởi đầu 2560/12/11
//
//  Xuất ảnh bất méo hóa

#import <stdio.h>
#import <stdlib.h>
#import <math.h>
#import <zlib.h>   // tải zlib


#define kSAI  0
#define kDUNG 1


/* đọc tệp PNG BGRO */
unsigned char *docPNG_BGRO( char *duongTapTin, unsigned int *beRong, unsigned int *beCao, unsigned char *canLatMau );

/* chỉnh sửa méo hóa */
unsigned char *chinhSuaMeoHoa( unsigned char *anh, unsigned int beRong, unsigned int beCao );

/* tô màu các điểm */
void toMauCacDiem( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned int *mangX, unsigned int *mangY, unsigned short soLuongDiem, unsigned int mau );

/* Lưu ảnh PNG */
void luuAnhPNG( char *tenTep, unsigned char *suKhacBiet, unsigned int beRong, unsigned int beCao );


int main( int argc, char **argv ) {

   if( argc > 1 ) {
      unsigned int beRongTep0;
      unsigned int beCaoTep0;
      unsigned char canLatMauTep0;
      unsigned char *duLieuTep0 = docPNG_BGRO( argv[1], &beRongTep0, &beCaoTep0, &canLatMauTep0 );

      if( duLieuTep0 ) {
         // ---- tính sự khác giữa hai ảnh
         unsigned char *anhChinhSuMeoHoa = chinhSuaMeoHoa( duLieuTep0, beRongTep0, beCaoTep0 );

         // ---- lưu sự khác
         char tenChinhSuaMeoHoa[255];
         char *dauTen = tenChinhSuaMeoHoa;
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
         *dauTen = '.';
         dauTen++;
         *dauTen = 'p';
         dauTen++;
         *dauTen = 'n';
         dauTen++;
         *dauTen = 'g';
         dauTen++;
         *dauTen = 0x00;

         printf( "Tên két qủa: %s\n", tenChinhSuaMeoHoa );
         luuAnhPNG( tenChinhSuaMeoHoa, anhChinhSuMeoHoa, beRongTep0, beCaoTep0 );

         free( anhChinhSuMeoHoa );
      }
      
      free( duLieuTep0 );
   }
   else {
      // ---- phàn nàn không có hình
      printf( "Làm ơn gõ tên tệp PNG BGRO.\n" );
   }
   return 1;
}




#pragma mark ---- Chỉnh Sửa Méo Hóa
// ---- Làm
//      1. kéo qua phải 72 - 68 = 4  ==> X = (2048 - x)*(2048 + 4)/2048
//      2. kéo xuống 1025 - 994 = 31 ==> Y = (y)*(994 + 31)/994
//      3. sửa cong quanh tâm:
//                tại giữa (x = 1024) --> (1025 - 994) --> 31 - 31 = 0 đã sửa rồi từ bươc 2
//                tại trái (x = 0) --> (1038 - 994) ---> 44 - 31 = 13
//                parabol y/994 * (x-1024)*(x-1024)*13/1024^2
unsigned char *chinhSuaMeoHoa( unsigned char *anh, unsigned int beRong, unsigned int beCao ) {
   
//   unsigned int tamX = 1200;
//   unsigned int tamY = 0;
   
   // ---- chỉnh sửa ảnh bậc một
   unsigned char *anhChinhSua0 = malloc( beRong*beCao << 2 );
   
   unsigned char *anhChinhSua1 = malloc( beRong*beCao << 2 );
   // ---- tính giá trị chỉnh
   
   if( anhChinhSua0 && anhChinhSua1 ) {

      unsigned int soHang = 0;
      while( soHang < beCao ) {
         int yDiemAnh = soHang*(994 + 31)/994;
         
         if( yDiemAnh > -1 && yDiemAnh < beCao ) {
            unsigned short soCot = 0;
            while( soCot < beRong ) {
               
               // ---- tính vị trí điểm ảnh thì cần
               int xDiemAnh = (2048 - soCot)*(2048 + 4)/2048;
               // ---- chỉ được tô nếu ờ trong phạm vi ảnh bị méo
               if( (xDiemAnh > -1) && (xDiemAnh < beRong) ) {
                  unsigned int diaChiAnhTrongAnhMeo = (yDiemAnh*beRong + xDiemAnh) << 2;
                  unsigned int diaChiAnh = (soHang*beRong + soCot) << 2;
                  unsigned char mauXanh = anh[diaChiAnhTrongAnhMeo];
                  unsigned char mauLuc = anh[diaChiAnhTrongAnhMeo+1];
                  unsigned char mauDo = anh[diaChiAnhTrongAnhMeo+2];

                  anhChinhSua0[diaChiAnh] = mauXanh;
                  anhChinhSua0[diaChiAnh+1] = mauLuc;
                  anhChinhSua0[diaChiAnh+2] = mauDo;
                  anhChinhSua0[diaChiAnh+3] = 0xff;
               }
               // ---- tô màu trắng
               else {
                  unsigned int diaChiAnh = (soHang*beRong + soCot) << 2;
                  anhChinhSua0[diaChiAnh] = 0xff;
                  anhChinhSua0[diaChiAnh+1] = 0xff;
                  anhChinhSua0[diaChiAnh+2] = 0xff;
                  anhChinhSua0[diaChiAnh+3] = 0xff;
               }
               soCot++;
            }
         }
         soHang++;
      }
      
      soHang = 0;
      while( soHang < beCao ) {
         printf( " --- %d: ", soHang );
         unsigned short soCot = 0;
         while( soCot < beRong ) {
            // ---- tính từng bước để tránh vấn đề ra ngòi phạm vi int
            int yDiemAnh = (soCot-1024)*(soCot-1024)/994;
            yDiemAnh *= soHang*13;
            yDiemAnh >>= 20;
            yDiemAnh += soHang;
//            int yDiemAnh = (soHang*13 * (soCot-1024)*(soCot-1024)/994) >> 20;
            if( (soCot == 0) || (soCot == 1024) )
            printf( "%d ", yDiemAnh );
            
            // ---- chỉ được tô nếu ờ trong phạm vi ảnh bị méo
            if( (yDiemAnh > -1) && (yDiemAnh < beCao) ) {
               unsigned int diaChiAnhTrongAnhMeo = (yDiemAnh*beRong + soCot) << 2;
               unsigned int diaChiAnh = (soHang*beRong + soCot) << 2;
               unsigned char mauXanh = anhChinhSua0[diaChiAnhTrongAnhMeo];
               unsigned char mauLuc = anhChinhSua0[diaChiAnhTrongAnhMeo+1];
               unsigned char mauDo = anhChinhSua0[diaChiAnhTrongAnhMeo+2];
               
               anhChinhSua1[diaChiAnh] = mauXanh;
               anhChinhSua1[diaChiAnh+1] = mauLuc;
               anhChinhSua1[diaChiAnh+2] = mauDo;
               anhChinhSua1[diaChiAnh+3] = 0xff;
            }
            // ---- tô màu trắng
            else {
               unsigned int diaChiAnh = (soHang*beRong + soCot) << 2;
               anhChinhSua1[diaChiAnh] = 0xff;
               anhChinhSua1[diaChiAnh+1] = 0xff;
               anhChinhSua1[diaChiAnh+2] = 0xff;
               anhChinhSua1[diaChiAnh+3] = 0xff;
            }
            soCot++;
         }
         soHang++;
         printf( "\n" );
      }
      free( anhChinhSua0 );
   }
   else {
      printf( "chinhSuaMeoHoa: Có vấn đề dành trí nhớ cho ảnh chỉnh sứa\n" );
   }
   
   return anhChinhSua1;
}

void toMauCacDiem( unsigned char *anh, unsigned int beRong, unsigned int beCao, unsigned int *mangX, unsigned int *mangY, unsigned short soLuongDiem, unsigned int mau ) {

   // ---- tô
   unsigned short chiSo = 0;
   while( chiSo < soLuongDiem ) {
      unsigned int diaChiAnh = (beRong*mangY[chiSo] + mangX[chiSo]) << 2;
      anh[diaChiAnh - 4] = 0;
      anh[diaChiAnh - 3] = 0;
      anh[diaChiAnh - 2] = 0xff;
      
      anh[diaChiAnh] = 0;
      anh[diaChiAnh+1] = 0;
      anh[diaChiAnh+2] = 0xff;
      
      anh[diaChiAnh + 4] = 0;
      anh[diaChiAnh + 5] = 0;
      anh[diaChiAnh + 6] = 0xff;
      
      // ----
      diaChiAnh -= beRong<<2;
      anh[diaChiAnh - 4] = 0;
      anh[diaChiAnh - 3] = 0;
      anh[diaChiAnh - 2] = 0xff;
      
      anh[diaChiAnh] = 0;
      anh[diaChiAnh + 1] = 0;
      anh[diaChiAnh + 2] = 0xff;
      
      anh[diaChiAnh + 4] = 0;
      anh[diaChiAnh + 5] = 0;
      anh[diaChiAnh + 6] = 0xff;
      
      // ----
      diaChiAnh += beRong<<3;
      anh[diaChiAnh - 4] = 0;
      anh[diaChiAnh - 3] = 0;
      anh[diaChiAnh - 2] = 0xff;
      
      anh[diaChiAnh] = 0;
      anh[diaChiAnh + 1] = 0;
      anh[diaChiAnh + 2] = 0xff;
      
      anh[diaChiAnh + 4] = 0;
      anh[diaChiAnh + 5] = 0;
      anh[diaChiAnh + 6] = 0xff;
      chiSo++;
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


