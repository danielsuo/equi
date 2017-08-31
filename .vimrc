:autocmd BufWritePost * silent! !mkdir -p build && cd build && cmake .. && make && cd ..

nmap <leader>r :redraw!<cr>
