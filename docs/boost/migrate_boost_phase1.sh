#!/bin/bash
#===============================================================================
# BOOST to C++23 Migration - Phase 1: Automated Trivial Replacements
#===============================================================================
# This script performs simple, low-risk replacements that can be done
# automatically without manual code review.
#
# Safe to run as it only changes well-known BOOST patterns.
#===============================================================================

set -e  # Exit on error

REPO_ROOT="/datos/proyectos/scantailor"
BACKUP_DIR="${REPO_ROOT}/.boost_migration_backup_$(date +%Y%m%d_%H%M%S)"

cd "$REPO_ROOT"

echo "=================================================="
echo "BOOST to C++23 Migration - Phase 1"
echo "=================================================="
echo ""
echo "Repository: $REPO_ROOT"
echo "Backup directory: $BACKUP_DIR"
echo ""

# Create backup
mkdir -p "$BACKUP_DIR"
echo "Creating backup of all files..."
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -exec cp --parents {} "$BACKUP_DIR/" \; 2>/dev/null || true

echo "✓ Backup created"
echo ""

#===============================================================================
# 1. FOREACH LOOPS - Replace BOOST_FOREACH with C++11 range-based for
#===============================================================================

echo "1. Replacing BOOST_FOREACH with range-based for loops..."

# Pattern: BOOST_FOREACH(variable, container)
# This regex works for simple cases where container doesn't have nested parentheses
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i \
    's/BOOST_FOREACH(\([^,]*\),\s*\([^)]*\))/for (\1 : \2)/g'

echo "✓ Replaced BOOST_FOREACH patterns"

#===============================================================================
# 2. Remove boost/foreach.hpp includes
#===============================================================================

echo "2. Removing boost/foreach.hpp includes..."

find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i '/#include <boost\/foreach\.hpp>/d'

echo "✓ Removed boost/foreach.hpp includes"

#===============================================================================
# 3. Smart Pointers: boost::shared_ptr → std::shared_ptr
#===============================================================================

echo "3. Replacing boost::shared_ptr with std::shared_ptr..."

# Replace includes
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/#include <boost\/shared_ptr\.hpp>/#include <memory>/g'

# Replace usage
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::shared_ptr/std::shared_ptr/g'

echo "✓ Replaced boost::shared_ptr"

#===============================================================================
# 4. Smart Pointers: boost::weak_ptr → std::weak_ptr
#===============================================================================

echo "4. Replacing boost::weak_ptr with std::weak_ptr..."

# Replace includes
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/#include <boost\/weak_ptr\.hpp>/#include <memory>/g'

# Replace usage
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::weak_ptr/std::weak_ptr/g'

echo "✓ Replaced boost::weak_ptr"

#===============================================================================
# 5. Scoped Arrays: boost::scoped_array → std::unique_ptr
#===============================================================================

echo "5. Replacing boost::scoped_array with std::unique_ptr..."

# Replace includes
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/#include <boost\/scoped_array\.hpp>/#include <memory>/g'

# Replace usage: boost::scoped_array<Type> → std::unique_ptr<Type[]>
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::scoped_array</std::unique_ptr</g'

# Note: Manual review needed for actual usage patterns
# (e.g., new int[10] → std::make_unique<int[]>(10) or similar)

echo "⚠️  Replaced boost::scoped_array patterns (manual review recommended)"

#===============================================================================
# 6. Arrays: boost::array → std::array
#===============================================================================

echo "6. Replacing boost::array with std::array..."

# Replace includes
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/#include <boost\/array\.hpp>/#include <array>/g'

# Replace usage
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::array/std::array/g'

echo "✓ Replaced boost::array"

#===============================================================================
# 7. Cstdint: boost/cstdint.hpp → cstdint
#===============================================================================

echo "7. Replacing boost/cstdint.hpp with standard cstdint..."

# Replace includes
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/#include <boost\/cstdint\.hpp>/#include <cstdint>/g'

# Replace boost::uint* and boost::int* types if they exist
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::uint8_t/uint8_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::uint16_t/uint16_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::uint32_t/uint32_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::uint64_t/uint64_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::int8_t/int8_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::int16_t/int16_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::int32_t/int32_t/g'
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::int64_t/int64_t/g'

echo "✓ Replaced boost/cstdint"

#===============================================================================
# 8. Type Traits: alignment_of
#===============================================================================

echo "8. Replacing boost::alignment_of with std::alignment_of..."

# Replace includes
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/#include <boost\/type_traits\/alignment_of\.hpp>/#include <type_traits>/g'

# Replace usage: boost::alignment_of<T>::value → std::alignment_of_v<T>
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" -print0 | \
    xargs -0 sed -i 's/boost::alignment_of<\([^>]*\)>::value/std::alignment_of_v<\1>/g'

echo "✓ Replaced boost::alignment_of"

#===============================================================================
# Summary and Validation
#===============================================================================

echo ""
echo "=================================================="
echo "Phase 1 Complete!"
echo "=================================================="
echo ""

# Count remaining BOOST references
REMAINING=$(find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" | \
    xargs grep -l "#include <boost/" 2>/dev/null | wc -l)

echo "Remaining files with BOOST includes: $REMAINING"

if [ $REMAINING -gt 0 ]; then
    echo ""
    echo "Files still containing BOOST includes:"
    find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" | \
        xargs grep -l "#include <boost/" 2>/dev/null | sort
fi

echo ""
echo "Next steps:"
echo "1. Review the changes: git diff"
echo "2. Compile and test: cd build && cmake .. && make"
echo "3. If compilation fails, check BOOST_DETAILED_CASES.md for manual fixes"
echo "4. Proceed with Phase 2 (Lambda/Bind) when ready"
echo ""
echo "To restore from backup if needed:"
echo "  cp -r $BACKUP_DIR/* ."
echo ""
