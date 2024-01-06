function is_controllable = check_controlability(A, B)
    n = size(A, 1);  % Ordem da matriz A
    C = B;  % Inicializa a matriz de controlabilidade com B

    % Construir a matriz de controlabilidade
    for i = 1:n-1
        C = [C, A^i * B];
    end

    % Calcula o posto da matriz de controlabilidade
    controlability_rank = rank(C);

    % Exibe o posto da matriz de controlabilidade
    disp(["Posto (rank) da matriz de controlabilidade: ", num2str(controlability_rank)]);

    % Verifica se o posto da matriz de controlabilidade é igual a n
    if controlability_rank == n
        is_controllable = true;
        disp("O sistema é controlável.");
    else
        is_controllable = false;
        disp("O sistema não é controlável.");
    end
end
