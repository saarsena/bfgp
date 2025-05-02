#!/bin/bash

# Perfect Git first commit and push script (with custom commit message)

# Check if inside a git repo
if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  echo "❌ Not inside a Git repository."
  exit 1
fi

# Unset upstream if it's broken
git branch --unset-upstream 2>/dev/null

# Create a README if none exists
if [ ! -f README.md ]; then
  echo "# New Repository" > README.md
  echo "📝 Created README.md"
fi

# Add everything
git add .

# Check if anything to commit
if git diff --cached --quiet; then
  echo "❌ Nothing to commit. Working tree is clean."
  exit 1
fi

# Ask for a custom commit message
echo "📝 Enter commit message (leave empty for 'initial commit'): "
read COMMIT_MSG

# Default if empty
if [ -z \"$COMMIT_MSG\" ]; then
  COMMIT_MSG=\"initial commit\"
fi

# Commit
git commit -m \"$COMMIT_MSG\"

# Detect current branch name
CURRENT_BRANCH=$(git symbolic-ref --short HEAD)

# Push and set upstream
git push -u origin \"$CURRENT_BRANCH\"

echo \"✅ First push complete. Tracking set: origin/$CURRENT_BRANCH\"

