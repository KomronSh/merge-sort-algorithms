# 📤 Инструкция по загрузке проекта на GitHub

## Шаг 1: Создайте репозиторий на GitHub

1. Перейдите на [GitHub.com](https://github.com)
2. Нажмите на иконку **+** в верхнем правом углу
3. Выберите **New repository**
4. **Заполните форму**:
   - **Repository name**: `merge-sort-algorithms` (или название на выбор)
   - **Description**: `Сравнение четырех алгоритмов сортировки со слиянием (двухфазная/однофазная, простое/естественное слияние)`
   - **Visibility**: Выберите **Public** (или **Private** если это приватный проект)
   - **Initialize this repository with**: Оставьте пусто (мы уже инициализировали локально)
5. Нажмите **Create repository**

## Шаг 2: Добавьте удаленный репозиторий

После создания репository на GitHub вы увидите инструкции. Скопируйте URL репозитория (похоже на `https://github.com/ВАШ_USERNAME/merge-sort-algorithms.git`) и выполните команду:

```powershell
cd "F:\study\24ва1\СиАКОД\Курсовая работа"
git remote add origin https://github.com/ВАШ_USERNAME/merge-sort-algorithms.git
git branch -M main
git push -u origin main
```

**Замените:**
- `ВАШ_USERNAME` на ваше имя пользователя GitHub
- `merge-sort-algorithms` на название вашего репозитория

## Шаг 3: Введите учетные данные (если требуется)

GitHub может попросить вас ввести:
- **Username**: Ваше имя пользователя GitHub
- **Password/Token**: Личный токен доступа (не пароль)

### Как создать Personal Access Token:

1. Перейдите в **Settings → Developer settings → Personal access tokens → Tokens (classic)**
2. Нажмите **Generate new token (classic)**
3. Выберите scopes: `repo` (полный доступ к репозиториям)
4. Нажмите **Generate token**
5. **Скопируйте токен** (он больше не будет видим)
6. Используйте этот токен вместо пароля

## ✅ Готово!

Ваш проект теперь загружен на GitHub! 

### Проверьте результат:

```powershell
git remote -v
```

Должно вывести:
```
origin  https://github.com/ВАШ_USERNAME/merge-sort-algorithms.git (fetch)
origin  https://github.com/ВАШ_USERNAME/merge-sort-algorithms.git (push)
```

### Посетите ваш репозиторий:
`https://github.com/ВАШ_USERNAME/merge-sort-algorithms`

---

## 📝 Полезные команды Git

### Обновить репозиторий после изменений:

```powershell
cd "F:\study\24ва1\СиАКОД\Курсовая работа"
git status              # Проверить статус
git add .               # Добавить все изменения
git commit -m "Описание изменений"  # Зафиксировать изменения
git push                # Загрузить на GitHub
```

### Пример коммитов для будущих обновлений:

```powershell
git commit -m "Add performance comparison analysis"
git commit -m "Fix bug in natural merge sort"
git commit -m "Optimize file I/O operations"
git commit -m "Update documentation with examples"
```

### Просмотр истории:

```powershell
git log --oneline       # Короткая история коммитов
git log --graph --all   # История с веточками
```

---

## 🔐 Безопасность

- ✅ Используйте **Personal Access Tokens** вместо пароля
- ✅ Никогда не коммитьте учетные данные или приватные ключи
- ✅ Файлы в `.gitignore` не будут загружены
- ✅ Измените название пользователя/email если нужно анонимность

---

## 🆘 Часто встречающиеся проблемы

### Проблема: "fatal: not a git repository"
**Решение**: Убедитесь, что вы в правильной директории
```powershell
cd "F:\study\24ва1\СиАКОД\Курсовая работа"
```

### Проблема: "fatal: 'origin' does not appear to be a 'git' repository"
**Решение**: Добавьте удаленный репозиторий
```powershell
git remote add origin https://github.com/USERNAME/repository.git
```

### Проблема: "Permission denied" при push
**Решение**: Проверьте токен доступа и правильность URL

### Проблема: Хотите переделать последний коммит
```powershell
git commit --amend --no-edit  # Добавить файлы к последнему коммиту
```

---

## 📚 Дополнительные ресурсы

- [GitHub Docs - Getting started](https://docs.github.com/en/get-started)
- [Git Book](https://git-scm.com/book/ru/v2)
- [GitHub CLI](https://cli.github.com/)

