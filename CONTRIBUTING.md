# Contributing to Spotify Controller ESP32

Thank you for considering contributing to this project!

## 📋 How to Contribute

### Reporting Bugs

Before creating bug reports, please check existing issues to avoid duplicates.

When creating a bug report:
- Use a clear and descriptive title
- Describe the exact steps to reproduce
- Provide expected vs actual behavior
- Include environment details (ESP32 model, display type, etc.)
- Add relevant logs/serial output

### Suggesting Enhancements

For feature suggestions:
- Use a clear title describing the feature
- Explain why this feature would be useful
- Provide implementation ideas if you have them
- Consider whether it fits the project scope

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Follow the code style guidelines
5. Commit with clear messages
6. Push to your fork (`git push origin feature/amazing-feature`)
7. Open a Pull Request

## 📝 Code Style

### C++ Guidelines
- Use `CamelCase` for classes
- Use `snake_case` for functions and variables
- Use `SCREAMING_SNAKE_CASE` for constants and macros
- Use meaningful names that describe purpose
- Keep functions focused and small

### Documentation
- Add Doxygen comments for public APIs
- Document non-obvious logic with inline comments
- Keep commit messages clear and descriptive

### Example
```cpp
/**
 * @brief Update the Spotify now playing information
 * @return true if update was successful
 */
bool updateNowPlaying() {
    // Implementation...
    return true;
}
```

## 🏗️ Architecture Guidelines

### Modular Design
- Keep components loosely coupled
- Use dependency injection where appropriate
- Avoid circular dependencies
- Follow existing patterns in the codebase

### Display Drivers
- Implement `DisplayInterface` for new display types
- Implement `TouchInterface` for new touch controllers
- Add to DisplayManager auto-detection logic
- Test with multiple display sizes if possible

### UI Components
- Follow Spotify-inspired design language
- Use the theme system for consistency
- Implement smooth transitions (200-300ms)
- Support multiple screen orientations

## 🧪 Testing

Before submitting:
- Test on actual hardware if possible
- Verify no memory leaks
- Check serial output for warnings/errors
- Ensure all features still work

## 📦 Commit Guidelines

### Commit Message Format
```
<type>: <subject>

<body>

<footer>
```

### Types
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Build process or auxiliary tool changes

### Example
```
feat: add support for ST7796 display

Added ST7796UDisplay driver with auto-detection.
Updated DisplayManager to handle 480x320 resolution.

Closes #123
```

## 📚 Documentation

When adding new features:
- Update README.md if user-facing
- Add inline documentation for new APIs
- Consider adding a wiki page for complex features

## 🤝 Code of Conduct

Be respectful, inclusive, and professional in all interactions.

## ❓ Questions?

Feel free to open a discussion for any questions!

---

Thank you for contributing! 🎵
