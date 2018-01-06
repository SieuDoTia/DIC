# DIC
Các chương trình giúp tương quan ảnh kỷ thuật số. Mục đích làm một hệ thống được hoạt động tự động.

Danh sách tệp:

Các tệp để làm video làm DIC. Để tất cả cùng nhau.
 - DIC.zip tệp blender để xuất video .AVI không nén
 - grating0039575.png
 - grating8002.png

 - FFT_Moi.c: để tính biến Fourier nhanh và nghịch biến
 - MeoHoa.c: cho gỡ bỏ sự méo hoá trong các ảnh
   • Để dùng nó, bạn cần chiếu và chục ảnh một độ thị bị méo và so nó với độ thị không bị méo để đặt được các tham số trong 
 
MeoHoa.c.
Hai tấm ảnh ví dụ góc để tính gợn sóng
 - Moire_00.png
 - Moire_10.png
 
==== Chương trình C cho Moiré ====
 - suKhacBiet.c:
 - suKhacBiet2.c: đang phát triển cách mới để kiếm các nét gợn sóng
 
   Biến dịch:
gcc -lz sukhacbietXam.c

   Chạy:
./a.out Moire_00.png Moire_10.png
