import { useState } from "react";

const FILES = {
  "main.c": {
    layer: "core",
    color: "#1a1a2e",
    accent: "#e94560",
    icon: "⚙️",
    title: "main.c — Vòng lặp chính & Điều phối trạng thái",
    summary: "Điểm khởi đầu duy nhất của toàn bộ chương trình. Quản lý vòng lặp game loop, máy trạng thái toàn cục (AppState), và phân phối dữ liệu dùng chung.",
    algorithms: [
      {
        name: "Khởi tạo hệ thống",
        detail: "Gọi InitWindow() tạo cửa sổ 1200×800, SetTargetFPS(60). Khởi tạo particle system qua InitParticles(). Load font UTF-8 cho tiếng Việt. Gọi InitLogin() — nếu thất bại thì thoát luôn."
      },
      {
        name: "Nạp dữ liệu chia sẻ",
        detail: "DocDuLieuTheBanDoc() → headBD (linked list BanDoc). DocDuLieuPhieuMuon() → headPM (linked list PhieuMuonNode). BuildMap(&mapPM, headPM) → xây bảng băm O(1) tra cứu. Đọc Doanhthu.txt → ThemDoanhThu() vào mapDT."
      },
      {
        name: "Máy trạng thái (State Machine)",
        detail: "Biến APP_STATE toàn cục điều khiển màn hình nào đang hiển thị. Vòng lặp while(!WindowShouldClose()) kiểm tra APP_STATE mỗi frame: LOGINAPP→LOGIN, HOME→màn chính, MANAGEBOOKS→sách, APP_MENU→menu nghiệp vụ, và 6 state con. Sau khi TraSach thanh toán → rebuild mapPM và mapDT."
      },
      {
        name: "Làm sạch tài nguyên",
        detail: "Khi thoát: UnloadFont, UnloadTexture cho avatar, FreeMemberList(headBD), GiaiPhongDoanhThuMap, free toàn bộ chuỗi headPM. Đảm bảo không rò rỉ bộ nhớ."
      }
    ],
    deps: ["login.c", "ManageBooks.c", "ManageUser.c", "Phieumuon.c", "TimkiemLSphieumuon.c", "Timkiemthethuvien.c", "Trasach.c", "IndanhsachPM.c", "Doanhthu.c", "menu.c", "libmanage.c"]
  },
  "login.c": {
    layer: "auth",
    color: "#16213e",
    accent: "#0f3460",
    icon: "🔐",
    title: "login.c — Xác thực tài khoản",
    summary: "Quản lý màn hình đăng nhập với danh sách liên kết đôi (doubly-linked list) cho tài khoản. Hỗ trợ hiển thị/ẩn mật khẩu và phản hồi trực quan.",
    algorithms: [
      {
        name: "Cấu trúc dữ liệu: Danh sách liên kết đôi",
        detail: "AccountList là linked list với node header. Mỗi Node có con trỏ next và prev. InsertAccount() chèn vào vị trí p (mặc định là header), gắn prev/next hai chiều — cho phép duyệt ngược khi lưu file."
      },
      {
        name: "LoginGetAccountData — Nạp tài khoản",
        detail: "Đọc file accounts.txt, dòng đầu chứa số lượng expectedCount. Mỗi dòng sscanf 6 trường (username|password|realName|dob|cccd|role). Gọi trim() dọn khoảng trắng. Đếm count == expectedCount thì trả TRUE, ngược lại FAILED."
      },
      {
        name: "CheckLogin — Xác minh đăng nhập",
        detail: "Duyệt linked list từ node đầu. So sánh username bằng strcmp(). Nếu khớp username → kiểm tra password → trả LOGIN_SUCCESS hoặc LOGIN_FAILED. Không tìm thấy username → LOGIN_FAILED."
      },
      {
        name: "LoginSaveAccountData — Lưu ngược",
        detail: "Để bảo toàn thứ tự file gốc: duyệt đến cuối list, sau đó đi ngược lại qua prev. Ghi từng tài khoản với padding UTF-8 (LEN_SHOW_USERNAME, v.v.) để canh cột đẹp."
      },
      {
        name: "UI mật khẩu",
        detail: "Khi không nhấn giữ nút 'hiện': dùng memset tạo chuỗi '*' dài bằng passwordBox->length. Khi giữ nút ShowPassword: vẽ text thật. BeginScissorMode cắt phần tràn ngoài ô input."
      }
    ],
    deps: ["libmanage.c"]
  },
  "libmanage.c": {
    layer: "util",
    color: "#0a3d62",
    accent: "#60a3bc",
    icon: "🛠️",
    title: "libmanage.c — Thư viện tiện ích dùng chung",
    summary: "Module nền tảng được include bởi hầu hết các file khác. Cung cấp particle system, xử lý input UTF-8, vẽ text tự xuống dòng và các helper UI.",
    algorithms: [
      {
        name: "Particle System — Nền động",
        detail: "Mảng AppParticles[MAX_PARTICLE] toàn cục. InitParticles(): mỗi hạt có vị trí ngẫu nhiên, vận tốc nhỏ (trôi lên trên). UpdateParticlesPosition(): cộng velocity mỗi frame, wrap quanh màn hình. DrawBackgroundParticles(): vẽ hình tròn mờ + nối các hạt gần nhau (<120px) bằng đường thẳng mờ — tạo hiệu ứng mạng lưới."
      },
      {
        name: "SetFontUTF8 — Font tiếng Việt",
        detail: "Truyền mảng codepoints gồm ASCII + tất cả ký tự có dấu tiếng Việt (gần 200 codepoint). Gọi LoadFontEx() với mảng này để Raylib render đúng tiếng Việt mà không cần font đặc biệt."
      },
      {
        name: "UpdateInputBox — Nhập liệu UTF-8",
        detail: "Hỗ trợ: Ctrl+V dán clipboard, Ctrl+Backspace xóa cả từ. Đếm số lệnh Backspace trong 1 frame (để Unikey hoạt động đúng — IME có thể gửi nhiều Backspace liên tiếp). GetCharPressed() nhận Unicode codepoint → CodepointToUTF8() → copy bytes vào buffer."
      },
      {
        name: "DrawTextAutoWrap — Xuống dòng tự động",
        detail: "Tách văn bản thành từng từ (dùng dấu cách và \\n làm phân cách). Đo chiều rộng từ bằng MeasureTextEx(). Nếu currentX + wordWidth > bounds.right thì xuống dòng mới. Nếu currentY vượt bounds.bottom thì dừng — không bao giờ tràn ra ngoài Rectangle."
      },
      {
        name: "lenStringUTF8 — Đếm ký tự UTF-8",
        detail: "Duyệt byte-by-byte. Byte nào có bit cao (0xC0) không bằng 0x80 thì là byte đầu của ký tự mới → đếm 1. Byte continuation (10xxxxxx) bỏ qua. Trả về số ký tự thực sự (khác strlen)."
      }
    ],
    deps: []
  },
  "menu.c": {
    layer: "nav",
    color: "#6a0572",
    accent: "#ff8bc3",
    icon: "🗂️",
    title: "menu.c — Menu nghiệp vụ (UI hồng pastel)",
    summary: "Vẽ màn hình menu với 5 nút nghiệp vụ chính. Trả về AppState cho main.c để điều hướng.",
    algorithms: [
      {
        name: "Scale linh hoạt đa độ phân giải",
        detail: "scale = min(screenW/1100, screenH/750). Mọi tọa độ, kích thước đều nhân với scale. Giới hạn scale >= 0.5f để không thu nhỏ quá mức. Layout tự căn giữa hoàn hảo ở mọi kích thước màn hình."
      },
      {
        name: "5 nút bố cục Grid tự động",
        detail: "4 nút xếp lưới 2×2 + 1 nút căn giữa hàng dưới. Tính gridW = btnW*2+gapX, gridH = btnH*3+gapY*2. startX = (screenW-gridW)/2, startY = headerH+(screenH-headerH-gridH)/2. Đảm bảo khối nút luôn căn giữa chính xác."
      },
      {
        name: "Hiệu ứng Hover Lift 3D",
        detail: "Khi hover: liftOffset = 6*scale px, trừ vào y của nút (nút nổi lên). shadowOffset tăng từ 6 lên 12. Bóng đổ vẽ trước nút. Chữ to hơn 2px. Màu nền đổi sang xanh nhạt. Tạo cảm giác 3D depth không cần shader."
      },
      {
        name: "Hiệu ứng thở (Pulsing title)",
        detail: "pulse = sinf(GetTime() * 3.0f) * 2.0f. Cộng vào titleSize → chữ tiêu đề dao động ±2px theo nhịp sin ~0.5Hz. Mềm mại, không gây mỏi mắt."
      }
    ],
    deps: []
  },
  "ManageBooks.c": {
    layer: "business",
    color: "#1e3a5f",
    accent: "#2980b9",
    icon: "📚",
    title: "ManageBooks.c — Quản lý sách (module độc lập)",
    summary: "Module riêng biệt với vòng lặp Raylib riêng. Quản lý danh sách sách, tác giả, thể loại, nhà xuất bản. Được gọi từ main.c như một 'màn hình con' hoàn chỉnh.",
    algorithms: [
      {
        name: "Vòng lặp độc lập",
        detail: "InitManageBooks() có while-loop riêng, chạy đến khi người dùng thoát. main.c gọi hàm này như gọi sub-application. Dùng máy trạng thái MANAGEBOOKS_STATE bên trong (Dashboard, AddBook, EditBook, Search...)."
      },
      {
        name: "Nạp dữ liệu phân tầng",
        detail: "Loadbooks() đọc dataTruyen.txt → BookList. LoadAuthor() đọc author.txt. LoadType() trích xuất từ BookList (không có file riêng). LoadPublisher() tương tự. LoadDescription() nạp mô tả bổ sung."
      },
      {
        name: "Tìm kiếm theo ngữ cảnh",
        detail: "g_searchContext là biến static lưu Tab đang active (Dashboard/AddBook/...). FindBar là InputBox chung. Kết quả tìm kiếm lọc theo context hiện tại — tránh tìm nhầm khi ở nhiều màn hình."
      }
    ],
    deps: ["libmanage.c"]
  },
  "ManageUser.c": {
    layer: "business",
    color: "#2d6a4f",
    accent: "#52b788",
    icon: "👤",
    title: "ManageUser.c — Tạo thẻ bạn đọc",
    summary: "Form tạo thẻ mới với 4 trường nhập liệu (họ tên, SĐT, CCCD, hạn sử dụng). Tự động sinh mã thẻ tăng dần.",
    algorithms: [
      {
        name: "Sinh mã thẻ tự động (LayMaTheTiepTheo)",
        detail: "Duyệt toàn bộ headBD, tìm ID lớn nhất hiện có. Mã thẻ mới = maxID + 1. Gán vào maThe và hiển thị trên form. Mỗi lần tạo thành công, currentTotalUsers++ và sinh mã tiếp theo."
      },
      {
        name: "UpdateFormPosition — Layout co giãn",
        detail: "Tính scale từ screenW/screenH. Card là Rectangle trung tâm. 4 ô input xếp dọc cách nhau spacing = 78*scale. Gán lại rec cho mỗi InputBox_BD mỗi frame → layout tự điều chỉnh khi resize cửa sổ."
      },
      {
        name: "Vòng lặp focus & Tab navigation",
        detail: "Mảng boxes[] gồm 4 InputBox. Click chuột → focus ô tương ứng. Nhấn TAB → focus chuyển sang ô (i+1)%4. Chỉ ô đang focus mới nhận ký tự từ GetCharPressed()."
      },
      {
        name: "Lưu file & hiển thị thành công",
        detail: "Ghi vào data/phieumuon/User.txt với padding UTF-8 (dùng UTF8Width). Sau khi lưu: showSuccess=true, successTimer=3s. Timer đếm ngược qua GetFrameTime(). Khi hết: reset form, sinh mã mới."
      }
    ],
    deps: ["libmanage.c", "menu.c"]
  },
  "Phieumuon.c": {
    layer: "business",
    color: "#7d3c98",
    accent: "#c39bd3",
    icon: "📋",
    title: "Phieumuon.c — Tạo phiếu mượn sách",
    summary: "Form 5 trường tạo phiếu mượn (mã thẻ, mã truyện, tên, ngày mượn/trả). Sinh mã PM tự động và duy trì linked list trong RAM.",
    algorithms: [
      {
        name: "SinhMaPM — Mã phiếu tự động",
        detail: "DemSoPhieuMuon() đếm độ dài headPM O(n). sprintf(mathe, 'PM%06d', count+1) → 'PM000001'. Mã sinh lại sau mỗi lần lưu thành công để luôn tăng dần không trùng lặp."
      },
      {
        name: "LuuPhieuMuonVaoFile",
        detail: "fopen mode 'a' (append). Trim khoảng trắng cuối mỗi trường. fprintf với padding UTF8Width để file giữ định dạng cột thẳng hàng. fflush() sau khi ghi — đảm bảo dữ liệu xuống đĩa ngay."
      },
      {
        name: "ThemPhieuMuonVaoList",
        detail: "malloc node mới. Copy toàn bộ trường từ Form vào node. trangthai = 0 (chưa trả). Chèn đầu danh sách: newNode->next = *head; *head = newNode. O(1), không cần duyệt."
      },
      {
        name: "Xử lý Backspace cho Unikey",
        detail: "Vòng while(GetKeyPressed()) đếm số lệnh Backspace trong frame. Thực hiện xóa đúng số lần đó. Nếu 0 lệnh nhưng đang giữ phím → xóa repeat sau 0.5s. Tránh bug IME xóa mất dấu tiếng Việt."
      }
    ],
    deps: ["libmanage.c", "menu.c"]
  },
  "TimkiemLSphieumuon.c": {
    layer: "business",
    color: "#154360",
    accent: "#5dade2",
    icon: "🔍",
    title: "TimkiemLSphieumuon.c — Hash Map phiếu mượn & Lịch sử",
    summary: "Xây dựng bảng băm (hash map) cho phiếu mượn theo mã thẻ. Vẽ modal lịch sử mượn sách với cuộn nội dung.",
    algorithms: [
      {
        name: "Cấu trúc Hash Map (PhieuMuonMang)",
        detail: "Mảng PM_HASH_SIZE bucket, mỗi bucket là linked list HashNodePM. HashMaThe(): atoi(mathe) % PM_HASH_SIZE — hash đơn giản dựa trên số nguyên của mã thẻ. Collision giải quyết bằng chaining (nối tiếp)."
      },
      {
        name: "BuildMap — Xây dựng bảng băm",
        detail: "InitPhieuMuonMang() reset toàn bộ bucket về NULL. Duyệt headPM O(n), gọi InsertPhieuMuonMang() cho từng node. Chèn đầu bucket O(1). Tổng O(n). Được gọi lại sau mỗi khi headPM thay đổi."
      },
      {
        name: "Layphantu — Tra cứu O(1) trung bình",
        detail: "Tính index = HashMaThe(mathe). Trả về map->mang[index]. Do collision chaining, caller phải lọc thêm strcmp(mathe) để loại node trùng hash khác mã."
      },
      {
        name: "DrawModalLichSuPhieuMuon — Modal cuộn",
        detail: "Vẽ overlay đen mờ toàn màn hình. Hiện modal Rectangle trung tâm. Đếm count phiếu khớp mã thẻ. Tính totalHeight = count * cardspacing. BeginScissorMode cắt vùng content. GetMouseWheelMove() cộng vào Form->modalscroll, clamp trong [contentH-totalH, 0]. Chỉ vẽ card trong vùng nhìn thấy."
      },
      {
        name: "DocDuLieuPhieuMuon — Đọc file",
        detail: "fgets bỏ dòng header đầu tiên. sscanf 7 trường (maPM|mathe|matruyen|tentruyen|ngaymuon|ngaytra|trangthai). trim() làm sạch. Chèn đầu list → cuối cùng đọc = đầu list (mới nhất hiển thị trên đầu)."
      }
    ],
    deps: ["libmanage.c", "Timkiemthethuvien.c"]
  },
  "Timkiemthethuvien.c": {
    layer: "business",
    color: "#0e6655",
    accent: "#76d7c4",
    icon: "🎴",
    title: "Timkiemthethuvien.c — Tìm kiếm thẻ bạn đọc",
    summary: "Tìm kiếm real-time trên linked list BanDoc. Hỗ trợ tìm không dấu tiếng Việt. Click thẻ → mở modal lịch sử phiếu mượn.",
    algorithms: [
      {
        name: "Chuẩn hóa không dấu UTF-8",
        detail: "ChuyenChuoiKhongDauVaThuong(): duyệt chuỗi UTF-8, gọi GetCodepoint() để decode từng ký tự. LayCodepointKhongDauVaThuong(): switch-case trên ~200 codepoint tiếng Việt (à,á,â,ã,ả,ạ,ắ,ặ...) → trả về ký tự ASCII tương ứng ('a','e','i','o','u','d'). Kết quả: chuỗi ASCII thường không dấu."
      },
      {
        name: "KiemTraKhopTimKiem — Tìm kiếm fuzzy",
        detail: "Chuẩn hóa hoTen, sdt, cccd và từ khóa. Dùng strstr() kiểm tra từ khóa có xuất hiện trong bất kỳ trường nào không. Trả true nếu khớp một trong ba. Nếu từ khóa rỗng → hiện tất cả."
      },
      {
        name: "DrawKetQuaTimKiem — Vẽ live search",
        detail: "Mỗi frame duyệt toàn bộ headBD O(n). Với mỗi BanDoc khớp: kiểm tra hover → vẽ viền cam nổi bật. Click → lưu dachonmathe, showmodal=true, modalscroll=0. Dùng BeginScissorMode từ y=toa_do_y_bat_dau xuống cuối màn hình để scroll không lấn header."
      },
      {
        name: "DocDuLieuTheBanDoc — Đọc user file",
        detail: "Bỏ qua dòng đầu (lineCount==1). sscanf 5 trường. atoi(mathe) để lấy ID số, cập nhật currenttotalusers nếu lớn hơn. Nối cuối list (duyệt đến temp->next==NULL) → giữ thứ tự file gốc."
      }
    ],
    deps: ["libmanage.c", "ManageUser.c", "menu.c"]
  },
  "Trasach.c": {
    layer: "business",
    color: "#6e2f1a",
    accent: "#e59866",
    icon: "📬",
    title: "Trasach.c — Xử lý trả sách & Tính tiền",
    summary: "Quy trình 2 bước: Tìm kiếm thẻ/phiếu → Xác nhận & tính tiền phạt. Ghi doanh thu và cập nhật file phiếu mượn.",
    algorithms: [
      {
        name: "Máy trạng thái 2 bước (FormTraSach.state)",
        detail: "TRA_B1_TIMKEM: hiện ô search, lọc headPM theo mathe chứa từ khóa, hiển thị danh sách phiếu chưa trả. Click phiếu → lưu phieuDangXuLy, chuyển TRA_B2_XACNHAN. TRA_B2_XACNHAN: nhập ngày trả thực tế, hiện tổng tiền. Bấm 'Thanh toán' → xử lý."
      },
      {
        name: "TinhTienThue — Công thức phạt/giảm",
        detail: "soNgayGoc = KhoangCachNgay(ngayMuon, ngayDuKien). tienGoc = giaMotNgay * soNgayGoc. lechNgay = KhoangCachNgay(ngayDuKien, ngayTraThucTe): =0 → đúng hạn (giữ nguyên), <0 → trả sớm (giảm 5%/ngày = tienGoc - gia*ngaySom*0.05), >0 → trả trễ (tăng 5%/ngày = tienGoc + gia*ngayTre*0.05)."
      },
      {
        name: "KhoangCachNgay — Tính ngày",
        detail: "ParseDateStr() chuyển 'DD/MM/YYYY' thành struct tm rồi mktime() → time_t (Unix timestamp). difftime(t2,t1) → giây. Chia (60*60*24) → số ngày chênh lệch. Âm = trả sớm."
      },
      {
        name: "laygiathue — Đọc giá từ file sách",
        detail: "Mở dataTruyen.txt, bỏ 3 dòng header. sscanf lấy mã sách cột đầu. Nếu khớp masachcantim: sscanf tiếp 11 trường bỏ qua (***) để lấy trường thứ 12 là giathue. Default 2000 nếu không tìm thấy."
      },
      {
        name: "Thanh toán & cập nhật",
        detail: "curr->trangthai = 1 (đã trả). LuuDoanhThuVaoFile() append ngày+tiền vào Doanhthu.txt. CapNhatToanBoFilePhieuMuon() ghi lại toàn bộ headPM (rewrite). Set formTraSach.daThanhToan=1 → main.c sẽ rebuild mapPM và reload mapDT."
      }
    ],
    deps: ["libmanage.c", "TimkiemLSphieumuon.c", "menu.c", "Doanhthu.c"]
  },
  "Doanhthu.c": {
    layer: "business",
    color: "#1a5276",
    accent: "#27ae60",
    icon: "💹",
    title: "Doanhthu.c — Dashboard Doanh Thu",
    summary: "Dashboard tổng hợp doanh thu theo Ngày/Tuần/Tháng/Quý/Năm từ hash map. Hỗ trợ lọc năm, cuộn danh sách.",
    algorithms: [
      {
        name: "Hash Map doanh thu (DoanhThuMap)",
        detail: "HashNgay(): polynomial rolling hash trên chuỗi ngày (hash = hash*31 + char) % DOANHTHU_HASH_SIZE. ThemDoanhThu(): nếu ngày đã tồn tại → cộng dồn tongTien (idempotent aggregation). Nếu chưa có → malloc node mới, chèn đầu bucket. Giải quyết collision bằng chaining."
      },
      {
        name: "Aggregation — Gộp theo kỳ thời gian",
        detail: "Duyệt toàn bộ hash map O(HASH_SIZE + n). Với mỗi node: sscanf ngày ra d/m/y. Tạo label theo currentView: VIEW_NGAY→'DD/MM/YYYY', VIEW_THANG→'Tháng MM/YYYY', VIEW_QUY→'Quý Q/YYYY', VIEW_NAM→'Năm YYYY'. VIEW_TUAN→tính diff ngày từ 01/01/2026, chia 7. Tìm label trong items[], nếu có → cộng dồn; nếu không → thêm mới. O(n²) do linear search nhưng n nhỏ (≤1000)."
      },
      {
        name: "Sắp xếp theo sortKey",
        detail: "Mỗi AggItem có sortKey: Ngày=y*10000+m*100+d, Tháng=y*100+m, Quý=y*10+q, Năm=y. Bubble sort O(n²) giảm dần (mới nhất lên đầu). Đơn giản nhưng đủ dùng cho dữ liệu nhỏ."
      },
      {
        name: "Scroll với Scissor",
        detail: "maxScroll=0, minScroll=contentH-totalH (âm). GetMouseWheelMove()*30 cộng vào form->scroll. Clamp trong [min,max]. BeginScissorMode cắt vùng danh sách. startY = listY + scroll → dịch toàn bộ nội dung. Chỉ vẽ dòng có startY trong vùng nhìn thấy."
      }
    ],
    deps: ["libmanage.c", "menu.c"]
  },
  "IndanhsachPM.c": {
    layer: "business",
    color: "#4a235a",
    accent: "#ffb6c1",
    icon: "📄",
    title: "IndanhsachPM.c — Danh sách phiếu mượn",
    summary: "Hiển thị toàn bộ phiếu mượn dưới dạng thẻ có thể cuộn. Lọc theo 2 tab: Tất cả / Chưa trả.",
    algorithms: [
      {
        name: "Tab filter & scroll reset",
        detail: "currentTab có 2 giá trị: VIEW_TAT_CA và VIEW_CHUA_TRA. Khi click tab khác → reset scrollList=0 để không bị lệch vị trí. Điều kiện lọc: thoaManDK = (tab==TAT_CA) || (tab==CHUA_TRA && trangthai==0)."
      },
      {
        name: "Tính giới hạn scroll động",
        detail: "Đếm count phiếu thỏa mãn điều kiện. totalHeight = count * (pmHeight + gap). minScroll = screenH - 180*scale - totalHeight (âm hoặc 0). Nếu nội dung ngắn hơn màn hình → minScroll=0 (không cuộn). Clamp scroll trong [min,0]."
      },
      {
        name: "Render 2 lớp (Layered drawing)",
        detail: "Lớp 1: BeginScissorMode từ y=160*scale, vẽ tất cả thẻ phiếu mượn có cuộn. Lớp 2: DrawRectangle đè lên vùng 0→160*scale bằng màu nền — che phần thẻ cuộn lên vượt header. Vẽ header (banner + tabs + nút back) lên trên cùng. Kỹ thuật masking tránh clip phức tạp."
      }
    ],
    deps: ["TimkiemLSphieumuon.c", "menu.c"]
  }
};

const LAYERS = [
  { id: "auth", label: "Xác thực", color: "#16213e", bg: "rgba(22,33,62,0.15)" },
  { id: "core", label: "Lõi hệ thống", color: "#1a1a2e", bg: "rgba(26,26,46,0.15)" },
  { id: "nav", label: "Điều hướng", color: "#6a0572", bg: "rgba(106,5,114,0.1)" },
  { id: "util", label: "Tiện ích chung", color: "#0a3d62", bg: "rgba(10,61,98,0.15)" },
  { id: "business", label: "Nghiệp vụ", color: "#1e3a5f", bg: "rgba(30,58,95,0.08)" },
];

const DATA_FLOWS = [
  { from: "main.c", label: "headBD (BanDoc*)", to: ["ManageUser.c", "Timkiemthethuvien.c", "Trasach.c"], color: "#52b788" },
  { from: "main.c", label: "headPM (PhieuMuonNode*)", to: ["Phieumuon.c", "TimkiemLSphieumuon.c", "IndanhsachPM.c", "Trasach.c"], color: "#5dade2" },
  { from: "main.c", label: "mapPM (Hash Map)", to: ["TimkiemLSphieumuon.c"], color: "#c39bd3" },
  { from: "main.c", label: "mapDT (Revenue Map)", to: ["Doanhthu.c"], color: "#27ae60" },
];

export default function App() {
  const [selected, setSelected] = useState("main.c");
  const [activeTab, setActiveTab] = useState("algo");
  const file = FILES[selected];

  const layerOrder = ["auth", "core", "nav", "util", "business"];
  const grouped = {};
  layerOrder.forEach(l => { grouped[l] = []; });
  Object.entries(FILES).forEach(([name, f]) => {
    if (grouped[f.layer]) grouped[f.layer].push(name);
  });

  return (
    <div style={{
      minHeight: "100vh",
      background: "linear-gradient(135deg, #0d0d1a 0%, #1a1a3e 50%, #0d1a2e 100%)",
      fontFamily: "'Segoe UI', system-ui, sans-serif",
      color: "#e8e8f0",
      display: "flex",
      flexDirection: "column",
    }}>
      {/* Header */}
      <div style={{
        padding: "20px 32px",
        borderBottom: "1px solid rgba(255,255,255,0.08)",
        background: "rgba(0,0,0,0.3)",
        backdropFilter: "blur(10px)",
      }}>
        <div style={{ fontSize: 11, letterSpacing: 4, color: "#888", marginBottom: 4, textTransform: "uppercase" }}>PBL1 — Library System</div>
        <h1 style={{ margin: 0, fontSize: 22, fontWeight: 700, color: "#fff" }}>
          Kiến trúc & Thuật toán — 12 Module C
        </h1>
        <div style={{ fontSize: 13, color: "#aaa", marginTop: 4 }}>Click vào bất kỳ module nào để xem chi tiết thuật toán</div>
      </div>

      <div style={{ display: "flex", flex: 1, overflow: "hidden", minHeight: 0 }}>
        {/* LEFT: Architecture Map */}
        <div style={{
          width: 340,
          minWidth: 300,
          background: "rgba(0,0,0,0.25)",
          borderRight: "1px solid rgba(255,255,255,0.07)",
          overflowY: "auto",
          padding: "20px 16px",
        }}>
          <div style={{ fontSize: 11, letterSpacing: 3, color: "#666", marginBottom: 16, textTransform: "uppercase" }}>Sơ đồ Module</div>

          {/* State flow */}
          <div style={{
            background: "rgba(233,69,96,0.1)",
            border: "1px solid rgba(233,69,96,0.3)",
            borderRadius: 8,
            padding: "10px 14px",
            marginBottom: 20,
            fontSize: 12,
            color: "#e94560",
          }}>
            <div style={{ fontWeight: 700, marginBottom: 6 }}>🔄 Luồng AppState</div>
            <div style={{ fontFamily: "monospace", color: "#aaa", lineHeight: 1.8 }}>
              LOGINAPP → HOME<br/>
              HOME → MANAGEBOOKS<br/>
              HOME → APP_MENU<br/>
              APP_MENU → [5 màn hình]<br/>
              APP_MENU → APP_DOANH_THU
            </div>
          </div>

          {layerOrder.map(layerId => {
            const layer = LAYERS.find(l => l.id === layerId);
            const files = grouped[layerId] || [];
            return (
              <div key={layerId} style={{ marginBottom: 16 }}>
                <div style={{
                  fontSize: 10,
                  letterSpacing: 2,
                  color: layer.color,
                  textTransform: "uppercase",
                  marginBottom: 6,
                  paddingLeft: 4,
                  borderLeft: `2px solid ${layer.color}`,
                  paddingLeft: 8,
                }}>
                  {layer.label}
                </div>
                <div style={{ display: "flex", flexDirection: "column", gap: 6 }}>
                  {files.map(name => {
                    const f = FILES[name];
                    const isSelected = selected === name;
                    return (
                      <button key={name} onClick={() => { setSelected(name); setActiveTab("algo"); }}
                        style={{
                          background: isSelected
                            ? `linear-gradient(135deg, ${f.accent}22, ${f.accent}44)`
                            : "rgba(255,255,255,0.04)",
                          border: isSelected
                            ? `1px solid ${f.accent}`
                            : "1px solid rgba(255,255,255,0.08)",
                          borderRadius: 8,
                          padding: "10px 14px",
                          cursor: "pointer",
                          color: isSelected ? "#fff" : "#bbb",
                          textAlign: "left",
                          display: "flex",
                          alignItems: "center",
                          gap: 10,
                          transition: "all 0.15s ease",
                          boxShadow: isSelected ? `0 0 12px ${f.accent}33` : "none",
                        }}>
                        <span style={{ fontSize: 18 }}>{f.icon}</span>
                        <div>
                          <div style={{ fontSize: 13, fontWeight: isSelected ? 700 : 500 }}>{name}</div>
                          <div style={{ fontSize: 10, color: isSelected ? f.accent : "#666", marginTop: 1 }}>
                            {f.algorithms.length} thuật toán
                          </div>
                        </div>
                        {isSelected && (
                          <div style={{ marginLeft: "auto", color: f.accent, fontSize: 16 }}>▶</div>
                        )}
                      </button>
                    );
                  })}
                </div>
              </div>
            );
          })}

          {/* Data flows */}
          <div style={{ marginTop: 20 }}>
            <div style={{ fontSize: 10, letterSpacing: 2, color: "#666", textTransform: "uppercase", marginBottom: 10, paddingLeft: 8, borderLeft: "2px solid #444" }}>
              Dữ liệu dùng chung
            </div>
            {DATA_FLOWS.map((flow, i) => (
              <div key={i} style={{
                background: "rgba(255,255,255,0.03)",
                border: `1px solid ${flow.color}44`,
                borderRadius: 6,
                padding: "8px 12px",
                marginBottom: 8,
                fontSize: 11,
              }}>
                <div style={{ color: flow.color, fontWeight: 700, marginBottom: 4 }}>{flow.label}</div>
                <div style={{ color: "#666", fontSize: 10 }}>
                  → {flow.to.join(", ")}
                </div>
              </div>
            ))}
          </div>
        </div>

        {/* RIGHT: Detail Panel */}
        <div style={{ flex: 1, overflowY: "auto", padding: "24px 32px" }}>
          {file && (
            <div>
              {/* File header */}
              <div style={{
                background: `linear-gradient(135deg, ${file.color}cc, rgba(0,0,0,0.4))`,
                border: `1px solid ${file.accent}44`,
                borderRadius: 12,
                padding: "24px 28px",
                marginBottom: 24,
                boxShadow: `0 4px 30px ${file.accent}22`,
              }}>
                <div style={{ display: "flex", alignItems: "flex-start", gap: 16 }}>
                  <div style={{
                    fontSize: 40,
                    background: `${file.accent}22`,
                    width: 64, height: 64,
                    display: "flex", alignItems: "center", justifyContent: "center",
                    borderRadius: 12,
                    border: `1px solid ${file.accent}44`,
                  }}>
                    {file.icon}
                  </div>
                  <div style={{ flex: 1 }}>
                    <div style={{
                      fontFamily: "monospace",
                      fontSize: 13,
                      color: file.accent,
                      marginBottom: 6,
                      letterSpacing: 1,
                    }}>{selected}</div>
                    <h2 style={{ margin: "0 0 8px", fontSize: 20, fontWeight: 700, color: "#fff", lineHeight: 1.3 }}>
                      {file.title.replace(selected + " — ", "")}
                    </h2>
                    <p style={{ margin: 0, fontSize: 14, color: "#aaa", lineHeight: 1.6 }}>{file.summary}</p>
                  </div>
                </div>

                {/* Dependencies */}
                {file.deps.length > 0 && (
                  <div style={{ marginTop: 16, paddingTop: 16, borderTop: "1px solid rgba(255,255,255,0.08)" }}>
                    <div style={{ fontSize: 11, color: "#666", marginBottom: 8, textTransform: "uppercase", letterSpacing: 2 }}>
                      Include / Phụ thuộc
                    </div>
                    <div style={{ display: "flex", flexWrap: "wrap", gap: 8 }}>
                      {file.deps.map(dep => (
                        <button key={dep} onClick={() => setSelected(dep)}
                          style={{
                            background: `${FILES[dep]?.accent || "#888"}22`,
                            border: `1px solid ${FILES[dep]?.accent || "#888"}55`,
                            borderRadius: 20,
                            padding: "4px 12px",
                            fontSize: 12,
                            color: FILES[dep]?.accent || "#aaa",
                            cursor: "pointer",
                            fontFamily: "monospace",
                            transition: "all 0.15s",
                          }}>
                          {dep}
                        </button>
                      ))}
                    </div>
                  </div>
                )}
              </div>

              {/* Tabs */}
              <div style={{ display: "flex", gap: 4, marginBottom: 20 }}>
                {[
                  { id: "algo", label: `📐 Thuật toán (${file.algorithms.length})` },
                  { id: "deps", label: "🔗 Liên kết" },
                ].map(tab => (
                  <button key={tab.id} onClick={() => setActiveTab(tab.id)} style={{
                    background: activeTab === tab.id ? file.accent : "rgba(255,255,255,0.05)",
                    border: "none",
                    borderRadius: 8,
                    padding: "8px 18px",
                    fontSize: 13,
                    color: activeTab === tab.id ? "#000" : "#aaa",
                    cursor: "pointer",
                    fontWeight: activeTab === tab.id ? 700 : 400,
                    transition: "all 0.15s",
                  }}>{tab.label}</button>
                ))}
              </div>

              {/* Algorithm list */}
              {activeTab === "algo" && (
                <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
                  {file.algorithms.map((algo, i) => (
                    <div key={i} style={{
                      background: "rgba(255,255,255,0.04)",
                      border: "1px solid rgba(255,255,255,0.08)",
                      borderRadius: 10,
                      padding: "20px 22px",
                      borderLeft: `3px solid ${file.accent}`,
                      transition: "all 0.2s",
                    }}>
                      <div style={{
                        display: "flex",
                        alignItems: "center",
                        gap: 10,
                        marginBottom: 12,
                      }}>
                        <div style={{
                          width: 26, height: 26,
                          borderRadius: "50%",
                          background: `${file.accent}33`,
                          border: `1px solid ${file.accent}77`,
                          display: "flex", alignItems: "center", justifyContent: "center",
                          fontSize: 12,
                          fontWeight: 700,
                          color: file.accent,
                          flexShrink: 0,
                        }}>{i + 1}</div>
                        <h3 style={{
                          margin: 0,
                          fontSize: 15,
                          fontWeight: 700,
                          color: "#fff",
                        }}>{algo.name}</h3>
                      </div>
                      <p style={{
                        margin: 0,
                        fontSize: 13.5,
                        color: "#ccc",
                        lineHeight: 1.75,
                        paddingLeft: 36,
                      }}>{algo.detail}</p>
                    </div>
                  ))}
                </div>
              )}

              {/* Deps tab */}
              {activeTab === "deps" && (
                <div>
                  <div style={{ marginBottom: 20 }}>
                    <div style={{ fontSize: 12, color: "#888", marginBottom: 12, textTransform: "uppercase", letterSpacing: 2 }}>
                      {selected} gọi / phụ thuộc vào:
                    </div>
                    {file.deps.length === 0 ? (
                      <div style={{ color: "#555", fontStyle: "italic", fontSize: 14 }}>Không phụ thuộc module nào (là nền tảng)</div>
                    ) : (
                      <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
                        {file.deps.map(dep => {
                          const df = FILES[dep];
                          return (
                            <button key={dep} onClick={() => setSelected(dep)} style={{
                              background: "rgba(255,255,255,0.04)",
                              border: `1px solid ${df?.accent || "#444"}44`,
                              borderRadius: 10,
                              padding: "14px 18px",
                              cursor: "pointer",
                              textAlign: "left",
                              display: "flex",
                              alignItems: "center",
                              gap: 14,
                              color: "#ccc",
                              transition: "all 0.15s",
                            }}>
                              <span style={{ fontSize: 24 }}>{df?.icon || "📄"}</span>
                              <div>
                                <div style={{ fontFamily: "monospace", color: df?.accent || "#aaa", fontWeight: 700 }}>{dep}</div>
                                <div style={{ fontSize: 12, color: "#666", marginTop: 2 }}>{df?.summary?.substring(0, 80)}...</div>
                              </div>
                              <div style={{ marginLeft: "auto", color: "#444" }}>→</div>
                            </button>
                          );
                        })}
                      </div>
                    )}
                  </div>

                  <div>
                    <div style={{ fontSize: 12, color: "#888", marginBottom: 12, textTransform: "uppercase", letterSpacing: 2 }}>
                      Các module phụ thuộc vào {selected}:
                    </div>
                    {(() => {
                      const dependents = Object.entries(FILES).filter(([n, f]) => f.deps.includes(selected)).map(([n]) => n);
                      return dependents.length === 0 ? (
                        <div style={{ color: "#555", fontStyle: "italic", fontSize: 14 }}>Không có module nào phụ thuộc vào đây</div>
                      ) : (
                        <div style={{ display: "flex", flexWrap: "wrap", gap: 8 }}>
                          {dependents.map(dep => (
                            <button key={dep} onClick={() => setSelected(dep)} style={{
                              background: `${FILES[dep]?.accent || "#888"}22`,
                              border: `1px solid ${FILES[dep]?.accent || "#888"}55`,
                              borderRadius: 20,
                              padding: "6px 14px",
                              fontSize: 13,
                              color: FILES[dep]?.accent || "#aaa",
                              cursor: "pointer",
                              fontFamily: "monospace",
                            }}>
                              {FILES[dep]?.icon} {dep}
                            </button>
                          ))}
                        </div>
                      );
                    })()}
                  </div>
                </div>
              )}
            </div>
          )}
        </div>
      </div>

      {/* Footer stats */}
      <div style={{
        padding: "12px 32px",
        borderTop: "1px solid rgba(255,255,255,0.06)",
        background: "rgba(0,0,0,0.3)",
        display: "flex",
        gap: 32,
        fontSize: 12,
        color: "#555",
      }}>
        <span>📦 12 module C</span>
        <span>🔗 Raylib graphics</span>
        <span>🗃️ File I/O (TXT)</span>
        <span>📊 Hash Map + Linked List</span>
        <span>🌐 UTF-8 / Tiếng Việt</span>
      </div>
    </div>
  );
}
