{
    git rev-list --objects --all
    git rev-list --objects -g --no-walk --all
    git rev-list --objects --no-walk \
        $(git fsck --unreachable |
          grep '^unreachable commit' |
          cut -d' ' -f3)
} | sort | uniq
