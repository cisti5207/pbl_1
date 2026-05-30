"""
Script tải ảnh bìa 117 truyện - Version 2
Dùng URL trực tiếp từ các nguồn đáng tin cậy (không cần search)
Chạy: python download_bia_truyen_v2.py
Ảnh lưu vào thư mục: ./bia_truyen/
"""

import requests
import os
import time

OUTPUT_DIR = "./bia_truyen"
os.makedirs(OUTPUT_DIR, exist_ok=True)

HEADERS = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36",
    "Accept": "image/webp,image/apng,image/*,*/*;q=0.8",
    "Accept-Language": "en-US,en;q=0.9",
    "Referer": "https://www.google.com/",
}

# Danh sách URL ảnh bìa trực tiếp cho 117 truyện
# Nguồn: MyAnimeList, AniList, Goodreads CDN, Wikipedia, OpenLibrary
COVERS = [
    # ── MANGA SHONEN / ACTION ──────────────────────────────────────────────
    ("T001", "Doraemon",
     "https://cdn.myanimelist.net/images/manga/2/163723l.jpg"),
    ("T002", "Thám tử lừng danh Conan",
     "https://cdn.myanimelist.net/images/manga/2/223078l.jpg"),
    ("T003", "One Piece",
     "https://cdn.myanimelist.net/images/manga/2/253146l.jpg"),
    ("T004", "Naruto",
     "https://cdn.myanimelist.net/images/manga/3/117981l.jpg"),
    ("T005", "Bảy viên ngọc rồng",
     "https://cdn.myanimelist.net/images/manga/1/267793l.jpg"),
    ("T006", "Attack on Titan",
     "https://cdn.myanimelist.net/images/manga/2/37846l.jpg"),
    ("T007", "Bleach",
     "https://cdn.myanimelist.net/images/manga/3/234093l.jpg"),
    ("T008", "Death Note",
     "https://cdn.myanimelist.net/images/manga/2/54453l.jpg"),
    ("T009", "Thanh gươm diệt quỷ",
     "https://cdn.myanimelist.net/images/manga/3/179023l.jpg"),
    ("T010", "Jujutsu Kaisen",
     "https://cdn.myanimelist.net/images/manga/3/210341l.jpg"),
    ("T011", "Chainsaw Man",
     "https://cdn.myanimelist.net/images/manga/3/216464l.jpg"),
    ("T012", "Hunter x Hunter",
     "https://cdn.myanimelist.net/images/manga/1/258245l.jpg"),
    ("T013", "Fairy Tail",
     "https://cdn.myanimelist.net/images/manga/3/214509l.jpg"),
    ("T014", "Black Clover",
     "https://cdn.myanimelist.net/images/manga/2/177838l.jpg"),
    ("T015", "Blue Lock",
     "https://cdn.myanimelist.net/images/manga/3/243745l.jpg"),
    ("T016", "Haikyuu!!",
     "https://cdn.myanimelist.net/images/manga/3/216737l.jpg"),
    # ── LIGHT NOVEL / TÂM LÝ ─────────────────────────────────────────────
    ("T017", "Classroom of the Elite",
     "https://cdn.myanimelist.net/images/manga/1/266291l.jpg"),
    ("T018", "Harry Potter",
     "https://covers.openlibrary.org/b/isbn/9780747532743-L.jpg"),
    # ── NAOKI URASAWA ─────────────────────────────────────────────────────
    ("T019", "20th Century Boys",
     "https://cdn.myanimelist.net/images/manga/2/210187l.jpg"),
    ("T020", "Monster",
     "https://cdn.myanimelist.net/images/manga/3/258224l.jpg"),
    ("T021", "Pluto",
     "https://cdn.myanimelist.net/images/manga/2/30905l.jpg"),
    ("T022", "Master Keaton",
     "https://cdn.myanimelist.net/images/manga/2/74351l.jpg"),
    ("T023", "Billy Bat",
     "https://cdn.myanimelist.net/images/manga/2/155060l.jpg"),
    # ── KIYOHIKO AZUMA ────────────────────────────────────────────────────
    ("T024", "Yotsuba&!",
     "https://cdn.myanimelist.net/images/manga/3/198272l.jpg"),
    ("T025", "Azumanga Daioh",
     "https://cdn.myanimelist.net/images/manga/1/267151l.jpg"),
    # ── OSAMU TEZUKA ──────────────────────────────────────────────────────
    ("T026", "Black Jack",
     "https://cdn.myanimelist.net/images/manga/1/267811l.jpg"),
    ("T027", "Dororo",
     "https://cdn.myanimelist.net/images/manga/2/176173l.jpg"),
    ("T028", "Astro Boy",
     "https://cdn.myanimelist.net/images/manga/1/268083l.jpg"),
    ("T029", "Hi Chim Sẻ Hello",
     "https://cdn.myanimelist.net/images/manga/2/259174l.jpg"),
    ("T030", "Phoenix",
     "https://cdn.myanimelist.net/images/manga/1/267809l.jpg"),
    ("T031", "Buddha",
     "https://cdn.myanimelist.net/images/manga/3/25145l.jpg"),
    # ── TRINH THÁM / LIGHT NOVEL ──────────────────────────────────────────
    ("T032", "Thám tử Kindaichi",
     "https://cdn.myanimelist.net/images/manga/2/189736l.jpg"),
    ("T033", "Sword Art Online",
     "https://cdn.myanimelist.net/images/manga/2/261835l.jpg"),
    ("T034", "Overlord",
     "https://cdn.myanimelist.net/images/manga/3/244353l.jpg"),
    ("T035", "No Game No Life",
     "https://cdn.myanimelist.net/images/manga/1/264341l.jpg"),
    ("T036", "Re:Zero",
     "https://cdn.myanimelist.net/images/manga/1/266985l.jpg"),
    ("T037", "Slime Chuyển Sinh",
     "https://cdn.myanimelist.net/images/manga/3/215895l.jpg"),
    # ── MAKOTO SHINKAI ────────────────────────────────────────────────────
    ("T038", "Your Name",
     "https://cdn.myanimelist.net/images/manga/1/264128l.jpg"),
    ("T039", "5 Centimet Trên Giây",
     "https://cdn.myanimelist.net/images/manga/3/188349l.jpg"),
    ("T040", "Weathering With You",
     "https://cdn.myanimelist.net/images/manga/1/264131l.jpg"),
    ("T041", "Suzume",
     "https://cdn.myanimelist.net/images/manga/1/264129l.jpg"),
    ("T042", "Khu Vườn Ngôn Từ",
     "https://cdn.myanimelist.net/images/manga/3/200085l.jpg"),
    ("T043", "Tiếng Gọi Từ Vì Sao",
     "https://cdn.myanimelist.net/images/manga/1/267810l.jpg"),
    # ── TRUYỆN TRUNG QUỐC ─────────────────────────────────────────────────
    ("T044", "Thế Giới Hoàn Mỹ",
     "https://s4.anilist.co/file/anilistcdn/media/manga/cover/large/bx85290-F7oNMpJXBvBi.jpg"),
    ("T045", "Toàn Chức Cao Thủ",
     "https://s4.anilist.co/file/anilistcdn/media/manga/cover/large/bx86287-qtaRWrGMsVMN.jpg"),
    ("T046", "Ma Đạo Tổ Sư",
     "https://s4.anilist.co/file/anilistcdn/media/manga/cover/large/bx104652-EEpEKcNYKlbE.jpg"),
    ("T047", "Thiên Quan Tứ Phúc",
     "https://s4.anilist.co/file/anilistcdn/media/manga/cover/large/bx124267-Jc36rXA3WVTF.jpg"),
    # ── HARUKI MURAKAMI ───────────────────────────────────────────────────
    ("T048", "Rừng Na Uy",
     "https://covers.openlibrary.org/b/isbn/9780375704024-L.jpg"),
    ("T049", "Kafka Bên Bờ Biển",
     "https://covers.openlibrary.org/b/isbn/9781400079278-L.jpg"),
    ("T050", "1Q84",
     "https://covers.openlibrary.org/b/isbn/9780307593313-L.jpg"),
    ("T051", "Biên Niên Ký Chim Vặn Dây Cót",
     "https://covers.openlibrary.org/b/isbn/9780679775430-L.jpg"),
    ("T052", "Phía Nam Biên Giới Phía Tây Mặt Trời",
     "https://covers.openlibrary.org/b/isbn/9780679748304-L.jpg"),
    ("T053", "Cuộc Săn Cừu Hoang",
     "https://covers.openlibrary.org/b/isbn/9780452264144-L.jpg"),
    # ── SELF-HELP / TRIẾT LÝ ─────────────────────────────────────────────
    ("T054", "Nghệ Thuật Tinh Tế",
     "https://covers.openlibrary.org/b/isbn/9780062457714-L.jpg"),
    ("T055", "Nhà Giả Kim",
     "https://covers.openlibrary.org/b/isbn/9780061122415-L.jpg"),
    ("T056", "Hoàng Tử Bé",
     "https://covers.openlibrary.org/b/isbn/9780156012195-L.jpg"),
    ("T057", "Kẻ Trộm Sách",
     "https://covers.openlibrary.org/b/isbn/9780375842207-L.jpg"),
    ("T058", "Suối Nguồn",
     "https://covers.openlibrary.org/b/isbn/9780451191151-L.jpg"),
    ("T059", "Giết Con Chim Nhại",
     "https://covers.openlibrary.org/b/isbn/9780061935466-L.jpg"),
    # ── NGUYỄN NHẬT ÁNH ──────────────────────────────────────────────────
    ("T060", "Mắt Biếc",
     "https://cdn0.fahasa.com/media/catalog/product/i/m/image_194807.jpg"),
    ("T061", "Cho Tôi Xin Một Vé Đi Tuổi Thơ",
     "https://cdn0.fahasa.com/media/catalog/product/i/m/image_194805.jpg"),
    ("T062", "Kính Vạn Hoa",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041044951.jpg"),
    ("T063", "Cô Gái Đến Từ Hôm Qua",
     "https://cdn0.fahasa.com/media/catalog/product/i/m/image_194808.jpg"),
    ("T064", "Trại Hoa Vàng",
     "https://cdn0.fahasa.com/media/catalog/product/i/m/image_194806.jpg"),
    ("T065", "Bồ Câu Không Đưa Thư",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261433.jpg"),
    ("T066", "Thằng Quỷ Nhỏ",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261457.jpg"),
    ("T067", "Bàn Có Năm Chỗ Ngồi",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261440.jpg"),
    ("T068", "Hạ Đỏ",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261464.jpg"),
    ("T069", "Thiên Thần Nhỏ Của Tôi",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261471.jpg"),
    ("T070", "Bảy Bước Tới Mùa Hè",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261525.jpg"),
    ("T071", "Con Chó Nhỏ Mang Giỏ Hoa Hồng",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261501.jpg"),
    ("T072", "Cảm Ơn Người Lớn",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261518.jpg"),
    ("T073", "Làm Bạn Với Bầu Trời",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261532.jpg"),
    ("T074", "Đảo Mộng Mơ",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261488.jpg"),
    ("T075", "Tôi Thấy Hoa Vàng Trên Cỏ Xanh",
     "https://cdn0.fahasa.com/media/catalog/product/i/m/image_194804.jpg"),
    ("T076", "Ngồi Khóc Trên Cây",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261495.jpg"),
    ("T077", "Chúc Một Ngày Tốt Lành",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261509.jpg"),
    ("T078", "Ngày Xưa Có Một Chuyện Tình",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261549.jpg"),
    # ── TÂM LÝ / KỸ NĂNG ─────────────────────────────────────────────────
    ("T079", "Thiên Tài Bên Trái Kẻ Điên Bên Phải",
     "https://cdn0.fahasa.com/media/catalog/product/9/7/9786041261556.jpg"),
    ("T080", "Muôn Kiếp Nhân Sinh",
     "https://cdn0.fahasa.com/media/catalog/product/m/u/muon-kiep-nhan-sinh.jpg"),
    ("T081", "Hành Trình Về Phương Đông",
     "https://cdn0.fahasa.com/media/catalog/product/h/a/hanh-trinh-ve-phuong-dong.jpg"),
    ("T082", "Hiểu Về Trái Tim",
     "https://cdn0.fahasa.com/media/catalog/product/h/i/hieu-ve-trai-tim.jpg"),
    ("T083", "Đắc Nhân Tâm",
     "https://covers.openlibrary.org/b/isbn/9780671723651-L.jpg"),
    ("T084", "Quốc Gia Khởi Nghiệp",
     "https://covers.openlibrary.org/b/isbn/9780446541473-L.jpg"),
    ("T085", "Từ Tốt Đến Vĩ Đại",
     "https://covers.openlibrary.org/b/isbn/9780066620992-L.jpg"),
    ("T086", "Suy Nghĩ Nhanh Và Chậm",
     "https://covers.openlibrary.org/b/isbn/9780374533557-L.jpg"),
    ("T087", "Lược Sử Loài Người",
     "https://covers.openlibrary.org/b/isbn/9780062316097-L.jpg"),
    ("T088", "Lược Sử Tương Lai",
     "https://covers.openlibrary.org/b/isbn/9780062464316-L.jpg"),
    ("T089", "21 Bài Học Cho Thế Kỷ 21",
     "https://covers.openlibrary.org/b/isbn/9780525512172-L.jpg"),
    ("T090", "Tôi Tự Học",
     "https://cdn0.fahasa.com/media/catalog/product/t/o/toi-tu-hoc.jpg"),
    ("T091", "Hải Thượng Y Tông Tâm Lĩnh",
     "https://cdn0.fahasa.com/media/catalog/product/h/a/hai-thuong-y-tong-tam-linh.jpg"),
    # ── VĂN HỌC VIỆT NAM ──────────────────────────────────────────────────
    ("T092", "Tắt Đèn",
     "https://cdn0.fahasa.com/media/catalog/product/t/a/tat-den-ngo-tat-to.jpg"),
    ("T093", "Chí Phèo",
     "https://cdn0.fahasa.com/media/catalog/product/c/h/chi-pheo-nam-cao.jpg"),
    ("T094", "Số Đỏ",
     "https://cdn0.fahasa.com/media/catalog/product/s/o/so-do-vu-trong-phung.jpg"),
    ("T095", "Dế Mèn Phiêu Lưu Ký",
     "https://cdn0.fahasa.com/media/catalog/product/d/e/de-men-phieu-luu-ky.jpg"),
    ("T096", "Đất Rừng Phương Nam",
     "https://cdn0.fahasa.com/media/catalog/product/d/a/dat-rung-phuong-nam.jpg"),
    ("T097", "Cánh Đồng Bất Tận",
     "https://cdn0.fahasa.com/media/catalog/product/c/a/canh-dong-bat-tan.jpg"),
    # ── TỨ ĐẠI DANH TÁC ──────────────────────────────────────────────────
    ("T098", "Tây Du Ký",
     "https://covers.openlibrary.org/b/isbn/9780520282018-L.jpg"),
    ("T099", "Tam Quốc Diễn Nghĩa",
     "https://covers.openlibrary.org/b/isbn/9780804834674-L.jpg"),
    ("T100", "Thủy Hử",
     "https://covers.openlibrary.org/b/isbn/9780253208163-L.jpg"),
    ("T101", "Hồng Lâu Mộng",
     "https://covers.openlibrary.org/b/isbn/9780140443714-L.jpg"),
    # ── VĂN HỌC PHƯƠNG TÂY ────────────────────────────────────────────────
    ("T102", "Xứ Cát",
     "https://covers.openlibrary.org/b/isbn/9780441013593-L.jpg"),
    ("T103", "Gatsby Vĩ Đại",
     "https://covers.openlibrary.org/b/isbn/9780743273565-L.jpg"),
    ("T104", "1984",
     "https://covers.openlibrary.org/b/isbn/9780451524935-L.jpg"),
    ("T105", "Trại Súc Vật",
     "https://covers.openlibrary.org/b/isbn/9780451526342-L.jpg"),
    ("T106", "Bắt Trẻ Đồng Xanh",
     "https://covers.openlibrary.org/b/isbn/9780316769174-L.jpg"),
    ("T107", "Mật Mã Da Vinci",
     "https://covers.openlibrary.org/b/isbn/9780307474278-L.jpg"),
    ("T108", "Thiên Thần Và Ác Quỷ",
     "https://covers.openlibrary.org/b/isbn/9781416524793-L.jpg"),
    ("T109", "Sự Im Lặng Của Bầy Cừu",
     "https://covers.openlibrary.org/b/isbn/9780312924584-L.jpg"),
    ("T110", "Rồng Đỏ",
     "https://covers.openlibrary.org/b/isbn/9780425228227-L.jpg"),
    ("T111", "Pháo Đài Số",
     "https://covers.openlibrary.org/b/isbn/9781250160454-L.jpg"),
    ("T112", "Điểm Dối Lừa",
     "https://covers.openlibrary.org/b/isbn/9781982125233-L.jpg"),
    ("T113", "Cuộc Đời Của Pi",
     "https://covers.openlibrary.org/b/isbn/9780156027328-L.jpg"),
    ("T114", "Chúa Tể Những Chiếc Nhẫn",
     "https://covers.openlibrary.org/b/isbn/9780618640157-L.jpg"),
    ("T115", "Trò Chơi Vương Quyền",
     "https://covers.openlibrary.org/b/isbn/9780553593716-L.jpg"),
    ("T116", "Cây Cam Ngọt Của Tôi",
     "https://covers.openlibrary.org/b/isbn/9781635573756-L.jpg"),
    ("T117", "Tiếng Chim Hót Trong Bụi Mận Gai",
     "https://covers.openlibrary.org/b/isbn/9780380018199-L.jpg"),
]


def download_image(ma, ten, url):
    """Tải ảnh từ URL và lưu theo mã truyện"""
    try:
        resp = requests.get(url, headers=HEADERS, timeout=20, allow_redirects=True)
        if resp.status_code != 200:
            return None, f"HTTP {resp.status_code}"
        if len(resp.content) < 1000:
            return None, f"File quá nhỏ ({len(resp.content)} bytes)"

        # Xác định đuôi file
        ct = resp.headers.get("content-type", "").lower()
        url_lower = url.lower().split("?")[0]
        if "png" in ct or url_lower.endswith(".png"):
            ext = ".png"
        elif "webp" in ct or url_lower.endswith(".webp"):
            ext = ".webp"
        elif "gif" in ct or url_lower.endswith(".gif"):
            ext = ".gif"
        else:
            ext = ".jpg"

        filepath = os.path.join(OUTPUT_DIR, f"{ma}{ext}")
        with open(filepath, "wb") as f:
            f.write(resp.content)
        return filepath, None
    except Exception as e:
        return None, str(e)


def main():
    print("=" * 60)
    print("  TẢI ẢNH BÌA 117 TRUYỆN - v2")
    print(f"  Lưu vào: {os.path.abspath(OUTPUT_DIR)}")
    print("=" * 60)

    success_list = []
    failed_list = []
    total = len(COVERS)

    for i, (ma, ten, url) in enumerate(COVERS):
        # Bỏ qua nếu đã có
        existing = [f for f in os.listdir(OUTPUT_DIR)
                    if f.startswith(ma + ".")]
        if existing:
            print(f"[{i+1:3d}/{total}] ✓ {ma} đã có: {existing[0]}")
            success_list.append(ma)
            continue

        print(f"[{i+1:3d}/{total}] Đang tải {ma} - {ten}...", end=" ", flush=True)
        filepath, err = download_image(ma, ten, url)

        if filepath:
            size = os.path.getsize(filepath)
            print(f"✓ ({size//1024} KB)")
            success_list.append(ma)
        else:
            print(f"✗ {err}")
            failed_list.append((ma, ten, err))

        time.sleep(0.3)  # nhẹ nhàng hơn vì không search nữa

    # Tổng kết
    print("\n" + "=" * 60)
    print(f"✅ Thành công : {len(success_list)}/{total}")
    print(f"❌ Thất bại   : {len(failed_list)}/{total}")
    if failed_list:
        print("\nDanh sách thất bại:")
        for ma, ten, err in failed_list:
            print(f"  {ma} - {ten}: {err}")
    print(f"\nẢnh lưu tại: {os.path.abspath(OUTPUT_DIR)}")


if __name__ == "__main__":
    main()