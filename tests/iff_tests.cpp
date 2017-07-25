
#include "iff.hpp"

#include <vector>

#include <cstddef>
#include <cstdint>

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

using byte_buffer = std::vector<std::byte>;

byte_buffer get_file_contents(const char* filename) {
    byte_buffer result;

    std::FILE* fp = std::fopen(filename, "rb");
    if (fp) {
        std::fseek(fp, 0, SEEK_END);
        result.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&result[0], 1, result.size(), fp);
        std::fclose(fp);
    } else {
        throw std::runtime_error("Invalid filename: " + std::string(filename));
    }

    return result;
}

template <typename StreamT>
void print_iff_chunk(StreamT& os, const esb::iff_chunk& chunk, int depth = 0) {
    os << std::string(depth, ' ') << chunk.id << " Size: " << chunk.size;

    if (esb::is_form(chunk)) {
        os << " Form Type: " << esb::get_form_type(chunk) << "\n";

        ++depth;

        for (auto& child : chunk) {
            print_iff_chunk(os, child, depth);
        }
    } else {
        os << "\n";
    }
}

TEST_CASE("CanReadInitialIffFormElement") {
    auto file_buffer = get_file_contents("data/a_wing_model.msh");
    auto chunk       = esb::create_chunk_from_raw(file_buffer.data(), file_buffer.size());

    REQUIRE(chunk.id.val == esb::TAG_FORM.val);
    REQUIRE(chunk.size == 208455);
}

TEST_CASE("CanReadFormType") {
    auto file_buffer = get_file_contents("data/a_wing_model.msh");
    auto chunk       = esb::create_chunk_from_raw(file_buffer.data(), file_buffer.size());

    REQUIRE(esb::get_form_type(chunk).val == 'HSEM');
}

TEST_CASE("CanGetFirstChildOfChunk") {
    auto file_buffer = get_file_contents("data/a_wing_model.msh");
    auto chunk       = esb::create_chunk_from_raw(file_buffer.data(), file_buffer.size());

    auto child = esb::get_first_child(chunk);

    REQUIRE(child);
    REQUIRE(child->id.val == esb::TAG_FORM.val);
    REQUIRE(child->size == 208443);
    REQUIRE(esb::get_form_type(*child).val == '5000');
}
