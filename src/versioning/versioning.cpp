#include <ios>
#include <stdexcept>
#include <string>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <iostream>


#include "boost/spirit/home/x3/char/char_class.hpp"
#include "boost/spirit/home/x3/core/parse.hpp"
#include "boost/spirit/home/x3/numeric/int.hpp"
#include <boost/spirit/home/x3.hpp>



namespace x3 = boost::spirit::x3;
namespace feature_versioning {

    
    enum class VersionComparisonResult : uint32_t {
        incompatible,
        compatible,
        identical
    };

    namespace default_comparisons {
        VersionComparisonResult semver_comp(std::string_view expected_ver_string, std::string_view provided_ver_string) {
            
            // define version string for semantic versioning (major.minor.patch)
            auto semver_parser_exp = -x3::lit('v') >> x3::int_ >> x3::lit('.') >> x3::int_ >> x3::lit('.') >> x3::int_;

            // parse expected version string
            std::vector<int> expected_version_numbers;
            auto expected_begin = expected_ver_string.begin();

            x3::phrase_parse(expected_begin, expected_ver_string.end(), semver_parser_exp, x3::ascii::space, expected_version_numbers);

            // parse provided version string
            std::vector<int> provided_version_numbers;
            auto provided_begin = provided_ver_string.begin();
           
            x3::phrase_parse(provided_begin, provided_ver_string.end(), semver_parser_exp, x3::ascii::space, provided_version_numbers);
            
            if (expected_begin != expected_ver_string.end()) {
                throw std::runtime_error("Invalid expected version format for semver parser!");
            }
            if (provided_begin != provided_ver_string.end()) {
                throw std::runtime_error("Invalid provided version format for semver parser!");
            }
            
            if (provided_version_numbers[0] != expected_version_numbers[0]) {
                return VersionComparisonResult::incompatible;
            }
            else if (provided_version_numbers[1] < expected_version_numbers[1]) {
                return VersionComparisonResult::incompatible;
            }
            else if (provided_version_numbers[2] != expected_version_numbers[2]) {
                return VersionComparisonResult::compatible;
            } 
            else {
                return VersionComparisonResult::identical;
            }
        };
    }

    namespace detail {
        VersionComparisonResult default_version_comparator(std::string_view ns, std::string_view expected_version, std::string_view provided_version) {
            if (ns == "semver") {
                return default_comparisons::semver_comp(expected_version, provided_version);
            }
            
            return VersionComparisonResult::incompatible;
        };
    }

    /**
     * @brief An individually named, namespaced and versioned component of a system
     * 
     * @tparam Type of comparator function, accepting namespace, expected version string and provided version string (all string_views)
     */
    template <typename VersionComparator = decltype(detail::default_version_comparator)*> requires requires (VersionComparator vc, std::string_view sv) {{vc(sv, sv, sv)} -> std::same_as<VersionComparisonResult>;}
    class Feature {
        std::string ns;
        std::string feature_name;
        std::string version;
        
        VersionComparator comparator;
        public:

        /**
         * @brief Construct a new Feature
         * 
         * @param feature_string A string of the form (namespace delimiter feature_name delimiter version_string).
         * @param vc Function vc(namespace, required, provided) that correctly compares the version any two features in a given namespace.
         * @param delimiter Delimiter between feature string fields. Defaults to "::".
         */
        explicit Feature(std::string_view feature_string, VersionComparator vc = detail::default_version_comparator, std::string_view delimiter = "::") : comparator{vc} {

            // example feature_string = namespace::feature_name::v1.0

            // extract position of first character of delimiter between fields
            auto ns_end_pos = feature_string.find(delimiter);
            auto name_end_pos = feature_string.find(delimiter, ns_end_pos + delimiter.size());

            if (ns_end_pos == std::string_view::npos || name_end_pos == std::string_view::npos) {
               std::string str = "Incorrectly delimited version string, with delimiter = ";
               throw std::runtime_error(str.append(delimiter)); 
            }
            // the ns_delim_pos doubles as the size, as the size is ns_delim_pos - 0
            ns = feature_string.substr(0, ns_end_pos);

            // compute feature name starting point
            auto name_begin_pos = ns_end_pos + delimiter.size();

            // remember to take the difference between the name end position and the name begin position, not the delimiter positions
            feature_name = feature_string.substr(name_begin_pos, name_end_pos - name_begin_pos);

            // the rest of the string after the feature name and delimiter is the version string
            version = feature_string.substr(name_end_pos + delimiter.size());
        };

        /**
         * @brief Tests if the provided feature satisfies the (current) required feature.
         * 
         * @param provided Provided feature
         * @return true - The provided feature satisfies the version requirement.
         * @return false - The provided feature does not satisfy the version requirement.
         */
        bool isSatisfiedBy(const Feature& provided) {
            if (provided.ns != ns || provided.feature_name != feature_name) {
                return false;
            }
            return static_cast<bool>(comparator(ns, version, provided.version));
        }

    };

};

int main() {

    feature_versioning::Feature required{"semver::common::v1.3.5"};
    feature_versioning::Feature provided{"semver::common::v1.3.7"};

    std::cout << std::boolalpha << required.isSatisfiedBy(provided) << std::endl;
}