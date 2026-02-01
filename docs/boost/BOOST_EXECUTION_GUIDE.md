# BOOST Elimination - Step-by-Step Execution Guide

## Pre-Flight Checklist

Before starting the migration, verify:

- [ ] Repository is clean (`git status` shows no uncommitted changes)
- [ ] You're on the `upgrade` branch
- [ ] CMakeLists.txt is readable and backed up
- [ ] Build system works (`cmake ..` and `make` successful)
- [ ] All tests pass before migration
- [ ] You have ~4-6 hours available without interruptions

```bash
# Verify status
cd /datos/proyectos/scantailor
git status
git branch
cmake --version
make --version
python3 --version
```

---

## Phase 1: Automated Trivial Replacements (2-4 hours)

### Step 1.1: Backup Everything

```bash
cd /datos/proyectos/scantailor

# Create manual backup
mkdir -p /tmp/boost_backup_$(date +%Y%m%d_%H%M%S)
cp -r . /tmp/boost_backup_$(date +%Y%m%d_%H%M%S)/

echo "✓ Backup created successfully"
```

### Step 1.2: Make Script Executable

```bash
chmod +x migrate_boost_phase1.sh
```

### Step 1.3: Review Script (Optional but Recommended)

```bash
# Look at what it will do
head -100 migrate_boost_phase1.sh
tail -50 migrate_boost_phase1.sh

# If comfortable, proceed. Otherwise, review the patterns:
grep "sed -i" migrate_boost_phase1.sh
```

### Step 1.4: Run the Script

```bash
# This creates another backup AND applies all replacements
./migrate_boost_phase1.sh

# Expected output:
# ================================================== 
# BOOST to C++23 Migration - Phase 1
# ==================================================
# 1. Replacing BOOST_FOREACH with range-based for loops...
# ✓ Replaced BOOST_FOREACH patterns
# 2. Removing boost/foreach.hpp includes...
# ✓ Removed boost/foreach.hpp includes
# ... (more replacements)
# Phase 1 Complete!
# ...
```

**Duration:** 1-2 minutes

### Step 1.5: Check What Changed

```bash
# See which files were modified
git status --short

# See detailed changes (warning: large output)
git diff --stat

# See BOOST references remaining
echo "Files with remaining BOOST includes:"
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" | \
    xargs grep -l "#include <boost/" 2>/dev/null | wc -l

echo "Specific includes still present:"
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" | \
    xargs grep "#include <boost/" 2>/dev/null | cut -d: -f2 | sort -u
```

### Step 1.6: Clean Build

```bash
cd build

# Clean previous build
make clean

# Reconfigure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Rebuild
make -j$(nproc)

# Expected time: 5-15 minutes (depending on machine)
# Expected result: Build succeeds OR shows specific compilation errors
```

### Step 1.7: Address Compilation Errors (If Any)

**Expected errors:**

1. **`#include "gui/...:` not found**
   - This is OK - Phase 2+ will handle it
   - Or manually fix by adding module name: `#include "imageproc/gui/..."`

2. **Undefined symbols**
   - Usually from incomplete scoped_array → unique_ptr migration
   - Check the specific error and fix based on context

3. **Type mismatches**
   - RARE after Phase 1 (should be binary compatible)
   - If occurs, check the modified files

**How to fix a compilation error:**

```bash
# Find the error in the build output
make 2>&1 | grep "error:"

# Locate the problematic file
# Edit it based on the error message
nano path/to/file.cpp

# Rebuild
make -j$(nproc)
```

### Step 1.8: Run Tests

```bash
# Run unit tests
ctest --output-on-failure

# Expected: Most tests pass
# Some tests might fail if they depend on BOOST.Test (that's Phase 3)
```

### Step 1.9: Commit Phase 1

```bash
# Review changes one final time
git diff --shortstat

# Stage all changes
git add -A

# Commit with clear message
git commit -m "Phase 1: Remove trivial BOOST dependencies (FOREACH, smart_ptr, arrays, etc.)

- Replaced BOOST_FOREACH with C++11 range-based for loops (78 files)
- Converted boost::shared_ptr/weak_ptr to std:: equivalents (3 files)
- Changed boost::scoped_array to std::unique_ptr (10 files)
- Updated boost::array to std::array (2 files)
- Replaced boost::cstdint with standard cstdint (2 files)
- Removed boost/type_traits includes and replaced with std:: (1 file)

Total: 70+ files updated, ~80 BOOST includes removed
Remaining BOOST includes: See next phases"

# Verify commit
git log -1
```

---

## Phase 1 Complete ✅

**What's accomplished:**
- ✅ ~70 files migrated to C++11/14 equivalents
- ✅ ~80 BOOST includes removed
- ✅ Build compiles and tests pass
- ✅ Low-risk changes committed

**What remains:**
- ⚠️ Lambda/Bind patterns (Phase 2)
- ⚠️ Function callbacks (Phase 2)
- ⚠️ Multi-index containers (Phase 3)
- ⚠️ Unit test framework (Phase 3)

**Lines of code changed:** ~2,000 total modifications across 70+ files

**Estimated time saved vs manual:** 3-4 hours of manual work automated

---

## Phase 2: Manual Lambda/Bind Refactoring (1-2 weeks)

### Prerequisites for Phase 2

```bash
# Verify Phase 1 completed successfully
git log --oneline | head -1
# Should show the Phase 1 commit

# Verify no broken builds
cd build && make clean && cmake .. && make -j$(nproc)
```

### Step 2.1: Identify Priority Files

Check the Phase 2 critical files:

```bash
# Most complex files requiring manual work
files=(
    "dewarping/gui/DetectVertContentBounds.cpp"
    "dewarping/gui/TextLineTracer.cpp"
    "filters/output/OutputGenerator.h"
    "foundation/gui/QtSignalForwarder.h"
)

for file in "${files[@]}"; do
    echo "=== $file ==="
    grep -c "#include <boost/" "$file" || echo "0"
done
```

### Step 2.2: Work Through Each Critical File

For each file in priority order:

1. Open the file
2. Review all `boost::lambda`, `boost::bind`, `boost::function` usage
3. Replace with appropriate std:: equivalent (see BOOST_DETAILED_CASES.md)
4. Compile and test
5. Commit with clear message

**Example for one file:**

```bash
# Open the file
nano dewarping/gui/DetectVertContentBounds.cpp

# Find all BOOST patterns
grep -n "boost::" dewarping/gui/DetectVertContentBounds.cpp

# Reference the case study in BOOST_DETAILED_CASES.md Section 2.1
# Make the replacements
# Test
cd build && make -j$(nproc)

# Commit
git add dewarping/gui/DetectVertContentBounds.cpp
git commit -m "Phase 2: Replace boost::lambda with std lambda in DetectVertContentBounds

- Replaced ll::_1 > threshold patterns with [](auto x) { return x > threshold; }
- Replaced ll::if_[] patterns with ternary operators
- Removed boost/lambda includes

Compile time: 2s (was 3s before - slight improvement)"
```

### Step 2.3: Systematic Approach

Work module by module:

1. **dewarping/** (Most complex - start here)
   - gui/DetectVertContentBounds.cpp
   - gui/TextLineTracer.cpp
   - gui/TowardsLineTracer.cpp

2. **filters/**
   - output/OutputGenerator.h
   - deskew/ImageView.cpp
   - etc.

3. **foundation/**
   - gui/QtSignalForwarder.h

4. **interaction/**
   - gui/InteractionHandler.cpp

Estimated time per file: 15-30 minutes

### Step 2.4: Verify No Regressions

After each file:

```bash
cd build && make -j$(nproc) && ctest --output-on-failure
```

---

## Phase 3: Complex Refactoring (2-3 weeks)

### Not recommended to do all at once

Phase 3 work is best done in parallel by multiple developers:

```
Developer 1: Multi-index containers refactoring
Developer 2: Unit test migration to Google Test
Developer 3: Intrusive list updates
```

### Step 3.1: Multi-Index Containers

Files to refactor:
- `src/FileNameDisambiguator.cpp`
- `filters/page_layout/Settings.cpp`
- `src/ProjectPages.cpp`
- etc. (7 total)

For each, follow BOOST_DETAILED_CASES.md Section 4:
- Create wrapper container class
- Replace multi_index_container with dual containers
- Update all access patterns
- Extensive testing

**Time per file:** 2-4 hours

### Step 3.2: Unit Test Framework Migration

Convert from Boost.Test to Google Test:

```bash
# Install Google Test (if not already)
sudo apt-get install libgtest-dev

# Go through each test file
# Convert BOOST_AUTO_TEST_CASE to TEST_F
# Convert BOOST_CHECK* to EXPECT_* or REQUIRE_*
# Run tests
```

**Time per test file:** 30-60 minutes

### Step 3.3: Intrusive Lists

Update foundation/ and related files:

```bash
# Replace boost::intrusive::list with std::list
# Or implement custom list if performance critical
```

**Time per file:** 1-2 hours

---

## Final Verification

Once all phases complete:

```bash
# 1. ZERO BOOST references should remain
echo "Remaining BOOST includes:"
find . -type f \( -name "*.cpp" -o -name "*.h" \) ! -path "*/build/*" ! -path "*/.git/*" | \
    xargs grep -c "#include <boost/" 2>/dev/null | grep -v ":0$"

# Expected output: (empty)

# 2. Clean rebuild
cd build
cmake ..
make clean
make -j$(nproc)

# 3. All tests pass
ctest --output-on-failure

# 4. Performance check (optional)
# Compare compile time and binary size before/after

# 5. Final commit message
git log --oneline | head -5
# Should show:
# Phase 3: ... (latest)
# Phase 2: ... 
# Phase 1: ... (oldest)
```

---

## Troubleshooting

### Problem: Script didn't complete

**Solution:**
```bash
# Check the backup
ls -la .boost_migration_backup_*/

# Restore from backup if needed
cp -r .boost_migration_backup_YYYYMMDD_HHMMSS/* .

# Try running the script again
./migrate_boost_phase1.sh
```

### Problem: Build fails after Phase 1

**Likely causes:**

1. Include paths broken
   - Check error message for `#include "..."` failures
   - These are OK for Phase 1 - Phase 2 will fix them

2. Type incompatibilities
   - Very rare
   - Check specific error, fix, rebuild

**Solution:**
```bash
# See the specific error
make 2>&1 | grep "error:" | head -20

# Fix based on the pattern
# Most common: add module prefix to includes
# e.g., #include "gui/..." → #include "imageproc/gui/..."
```

### Problem: Tests fail after Phase 1

**Likely:** Some tests use Boost.Test framework (Phase 3 work)

**Solution:**
```bash
# This is expected
# Boost.Test failures = Phase 3 work
# Other test failures = need debugging

# Run specific test
ctest -R TestName -VV

# Check test source to understand failure
```

### Problem: Unclear which boost:: references need fixing

**Solution:**

Check BOOST_CHEATSHEET.md or BOOST_DETAILED_CASES.md for your specific pattern.

---

## Rollback at Any Point

```bash
# See all backups created
ls -la .boost_migration_backup_*/

# Restore from latest backup
latest=$(ls -td .boost_migration_backup_* | head -1)
cp -r $latest/* .

# Reset git
git reset --hard
git clean -fd

# You're back to pre-migration state
```

---

## Estimated Timeline for Full Team

| Phase | Task | Effort | Team | Duration |
|-------|------|--------|------|----------|
| 1 | Automated script | 2h | 1 person | 1 day |
| 2a | Lambda/Bind in dewarping/ | 8h | 1 person | 4 days |
| 2b | Lambda/Bind in filters/ | 10h | 1 person | 5 days |
| 2c | Lambda/Bind in other modules | 8h | 1 person | 4 days |
| 3a | Multi-index containers | 16h | 1 person | 1 week |
| 3b | Unit test migration | 12h | 1 person | 1 week |
| 3c | Intrusive lists + cleanup | 8h | 1 person | 4 days |
| Verification | Testing + final touches | 8h | Team | 3 days |
| **TOTAL** | **Full elimination of BOOST** | **72h** | **2-3 devs** | **4 weeks** |

---

## Success Criteria Checklist

Phase 1:
- [ ] Script ran without errors
- [ ] Build succeeds
- [ ] Tests pass (except Boost.Test ones)
- [ ] Committed to git
- [ ] BOOST includes reduced from 258 to ~100

Phase 2:
- [ ] All lambda/bind patterns replaced
- [ ] Build succeeds with no BOOST warnings
- [ ] Tests pass
- [ ] Code reviewed
- [ ] BOOST includes reduced to ~20

Phase 3:
- [ ] Zero BOOST includes remaining
- [ ] All tests pass (with new framework)
- [ ] Build succeeds cleanly
- [ ] Final code review and merge

---

## Post-Migration

Once complete:

```bash
# Update CMakeLists.txt to remove BOOST detection
# (currently BOOST is not even found, so this is moot,
#  but document that BOOST is no longer needed)

# Update build documentation
# Update CI/CD pipeline if needed
# Tag final commit
git tag -a "v-boost-elimination-complete" -m "All BOOST dependencies removed"

# Generate final report
./build/scantailor --version
```

---

## Questions During Execution?

1. **For Phase 1 issues**: Check script output, look at actual file changes
2. **For Phase 2 patterns**: Reference BOOST_DETAILED_CASES.md
3. **For Phase 3 architecture**: Check BOOST_TO_CPP23_MIGRATION.md Section 4-6
4. **For test migration**: See BOOST_TO_CPP23_MIGRATION.md Section 10

---

**Good luck with the migration!** 🚀
