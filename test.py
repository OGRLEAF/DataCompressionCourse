from cProfile import label
from mimetypes import init
import ctypes as C
import os
import sys

os.system("gcc -o ./build/cmp.so -I ./lib/include --share -fPIC ./cmp.c ./lib/src/*")
print("Compare")
dll = C.CDLL("./build/cmp.so")
dll.cmp.restype = C.c_double
dll.cmp.argtypes = [C.c_char_p, C.c_char_p]
PSNR = dll.cmp(b"./test/Birds.bmp", b"./test/test_out/bit_2/Birds/Birds.dpcm.dhuff.bmp")
print(PSNR)

def calc_psnr(path_a:str, path_b:str):
    return dll.cmp(path_a.encode(encoding="utf-8"), path_b.encode(encoding="utf-8"))

huff_bin = "./Huffman_test/huff_run.exe"

bits = 8
D = 9 - bits
bmps = ["Birds", "Camman256B", "Clown256B", "Fruit256B", "Lena256B", "Noise256B", "Odie256B", "Zone256B"]
BASE_PATH = "./test"
BASE_OUT_PATH = f"{BASE_PATH}/test_out/bit_{bits}"
MD_BASE_URL = ""

os.system(f"mkdir -p {BASE_OUT_PATH}")
report_file = open(f"{BASE_OUT_PATH}/report.csv", "w+")
md_file = open(f"{BASE_OUT_PATH}/report.md", "w+")
md_file.write("| 图片|原图| 原体积(B) | 熵编码(B) | 压缩比|DPCM+熵编码| 压缩结果| 压缩比|PSNR|\r\n|-|-|-|-|-|-|-|-|-|\r\n")
class Test:
    def __init__(self, name:str, d:int) -> None:
        self.d = d
        self.name = name
        self.test_file_out_dir = f"{BASE_OUT_PATH}/{name}"
        self.input_bmp_file_path = f"{BASE_PATH}/{name}.bmp"
        self.output_dpcm_file_path = f"{self.test_file_out_dir}/{name}.dpcm"
        self.output_dpcm_huff_path = f"{self.output_dpcm_file_path}"
        self.output_huff_path = f"{self.test_file_out_dir}/{name}"
        pass

    def excute(self):
        os.system(f"mkdir -p {self.test_file_out_dir}")
        os.system(f"rm {self.test_file_out_dir}/* -Rf")
        os.system(f"echo start")
        os.system(f"./build/main {self.input_bmp_file_path} {self.output_dpcm_file_path} {self.d}")


        os.system(f"wine {huff_bin} -i {self.output_dpcm_file_path} -o {self.output_dpcm_file_path}.huff -c -t {self.output_dpcm_file_path}.txt")
        os.system(f"wine {huff_bin} -i {self.input_bmp_file_path} -o {self.output_huff_path}.huff -c -t {self.output_huff_path}.txt")

        os.system(f"wine {huff_bin} -i {self.output_dpcm_file_path}.huff  -o {self.output_dpcm_file_path}.dhuff -d")
        os.system(f"./build/main_d {self.output_dpcm_file_path}.dhuff {self.output_dpcm_file_path}.dhuff.bmp {self.d}")

        os.system(f"ls -l {self.test_file_out_dir}")

    def result(self):
        origin_size = os.path.getsize(self.input_bmp_file_path)
        origin_huff_size = os.path.getsize(f"{self.output_huff_path}.huff")
        origin_huff_rate = origin_huff_size/origin_size
        dpcm_huff_size = os.path.getsize(f"{self.output_dpcm_huff_path}.huff")
        dpcm_huff_rate = dpcm_huff_size/origin_size
        res = f"{self.name}\t{origin_size}\t{origin_huff_size}\t{origin_huff_rate:.4}\t{dpcm_huff_size}\t{dpcm_huff_rate:.4}"
        report_file.write(res + "\r\n")

        md_img = lambda p:f"![{self.name}]({MD_BASE_URL}{p})";        
        dpcm_out_md = md_img(self.output_dpcm_file_path + ".dhuff.bmp")

        PSNR = calc_psnr(self.input_bmp_file_path, self.output_dpcm_file_path + ".dhuff.bmp")
        md_chart = f"|{self.name}|{md_img(self.input_bmp_file_path)}|{origin_size}|{origin_huff_size}|{origin_huff_rate:.4}|{dpcm_huff_size}|{dpcm_out_md}|{dpcm_huff_rate:.4}|{PSNR:.4}\r\n"
        md_file.write(md_chart)


for bmp in bmps:
    test = Test(bmp, D)
    # test.excute()
    test.result()