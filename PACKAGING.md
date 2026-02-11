# PyPI Packaging Guide for cia-to-cci

This guide explains how to package and publish this project to PyPI.

## Prerequisites

1. **PyPI Account**: Create accounts on:
   - [Test PyPI](https://test.pypi.org/account/register/) - for testing
   - [PyPI](https://pypi.org/account/register/) - for production

2. **API Tokens**: Generate API tokens:
   - Go to PyPI Account Settings → API tokens
   - Create a token with upload permissions
   - Save it securely (you won't see it again)

3. **GitHub Secrets**: Add your PyPI token to GitHub:
   - Go to your repository → Settings → Secrets and variables → Actions
   - Add a new secret named `PYPI_API_TOKEN`
   - Paste your PyPI API token

## Local Testing

### 1. Test Build Locally

```bash
# Install build tools
pip install build twine cibuildwheel

# Build source distribution
python -m build --sdist

# Build wheel for your current platform
python -m build --wheel

# Check the built packages
twine check dist/*
```

### 2. Test Installation Locally

```bash
# Install from the built wheel
pip install dist/cia_to_cci-0.1.0-*.whl

# Test it works
python -c "from cia_to_cci import convert_cia_to_cci; print('Success!')"
```

### 3. Test with cibuildwheel (Optional)

```bash
# Build wheels for all platforms (takes a while)
cibuildwheel --platform linux

# Or just your platform
cibuildwheel --platform macos
```

## Publishing to Test PyPI (Recommended First)

### Manual Upload

```bash
# Build the package
python -m build

# Upload to Test PyPI
twine upload --repository testpypi dist/*
# Enter your Test PyPI credentials when prompted

# Test installation from Test PyPI
pip install --index-url https://test.pypi.org/simple/ cia-to-cci
```

### Using GitHub Actions

1. Edit `.github/workflows/build-and-publish.yml`
2. Uncomment the line:
   ```yaml
   repository-url: https://test.pypi.org/legacy/
   ```
3. Add `PYPI_API_TOKEN` secret with your **Test PyPI** token
4. Push a tag:
   ```bash
   git tag v0.1.0-test
   git push origin v0.1.0-test
   ```

## Publishing to Production PyPI

### Option 1: Automated via GitHub Actions (Recommended)

1. Ensure `PYPI_API_TOKEN` is set in GitHub Secrets (with **production** PyPI token)
2. Make sure `.github/workflows/build-and-publish.yml` is configured for production
3. Create and push a version tag:

   ```bash
   # Update version in pyproject.toml first
   git add pyproject.toml
   git commit -m "Bump version to 0.1.0"
   git tag v0.1.0
   git push origin main
   git push origin v0.1.0
   ```

4. GitHub Actions will:
   - Build wheels for macOS (x86_64 and arm64) and Linux (x86_64)
   - Build source distribution
   - Publish to PyPI automatically

### Option 2: Manual Upload

```bash
# Clean previous builds
rm -rf dist/ build/ *.egg-info

# Build fresh packages
python -m build

# Upload to PyPI
twine upload dist/*
```

## Version Management

Update version in these files:
- `pyproject.toml` - line 12: `version = "0.1.0"`

Follow [Semantic Versioning](https://semver.org/):
- **Major** (1.0.0): Breaking changes
- **Minor** (0.1.0): New features, backward compatible
- **Patch** (0.0.1): Bug fixes

## Wheel Building Details

### Supported Platforms

The project builds wheels for:
- **macOS**: x86_64 and arm64 (Apple Silicon)
- **Linux**: x86_64 (manylinux)
- **Python**: 3.8, 3.9, 3.10, 3.11, 3.12

### Build Requirements

Each wheel includes:
- Compiled C/C++ extensions (_ctrtool, _makerom)
- All dependencies statically linked
- No external runtime dependencies

### Platform-Specific Notes

**macOS:**
- Universal2 wheels support both Intel and Apple Silicon
- Requires Xcode Command Line Tools for building

**Linux:**
- Uses manylinux2014 for broad compatibility
- Statically links libstdc++ and other dependencies

## Troubleshooting

### "Failed to build extension"

Ensure you have:
```bash
# macOS
brew install cmake

# Linux
sudo apt-get install cmake build-essential python3-dev
```

### "Submodule not initialized"

Always clone with submodules:
```bash
git clone --recursive https://github.com/reyrodrigues/cia-to-cci.git
```

Or initialize after cloning:
```bash
git submodule update --init --recursive
```

### "Wheel is not compatible"

Check wheel compatibility:
```bash
unzip -l dist/*.whl | grep -E '\.so|\.dylib'
```

Verify it works on target platform before uploading.

## Pre-Release Checklist

Before publishing a new version:

- [ ] All tests pass: `pytest tests/`
- [ ] Version updated in `pyproject.toml`
- [ ] CHANGELOG updated (if you have one)
- [ ] README is up to date
- [ ] Built and tested locally
- [ ] Tested on Test PyPI
- [ ] Git tag created and pushed
- [ ] GitHub Actions build successful

## Post-Release

After publishing:

1. Verify installation works:
   ```bash
   pip install cia-to-cci
   python -c "from cia_to_cci import convert_cia_to_cci; print('Success!')"
   ```

2. Check the PyPI page: https://pypi.org/project/cia-to-cci/

3. Create a GitHub Release:
   - Go to Releases → Create new release
   - Tag: v0.1.0
   - Title: "Release 0.1.0"
   - Describe changes

## Resources

- [PyPI Publishing Guide](https://packaging.python.org/tutorials/packaging-projects/)
- [cibuildwheel Documentation](https://cibuildwheel.readthedocs.io/)
- [GitHub Actions for PyPI](https://github.com/marketplace/actions/pypi-publish)
- [Semantic Versioning](https://semver.org/)
