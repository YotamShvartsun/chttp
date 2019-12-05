#include <chttp/util/MultipartField.h>

int main(int argc, char **argv) {
    std::string ds = "---a\r\nContent-Disposition: form-data; name=\"file1\"; filename=\"a.txt\"\r\nContent-Type: text/plain\r\n\r\naaa\r\n-----a";
    std::vector<char> d(ds.begin(), ds.end());
    std::string ct = "multipart/form-data; boundary=---a";
    std::vector<char> c(ct.begin(), ct.end());
    MultipartField f(d, ct);
    return 0;
}